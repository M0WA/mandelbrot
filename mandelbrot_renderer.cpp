#include "mandelbrot_renderer.h"
#include "mandelbrot_calculator.h"
#include "info_window.h"
#include <iostream>
#include <cmath>
#include <thread>
#include <vector>

MandelbrotGLRenderer* MandelbrotGLRenderer::instance = nullptr;

MandelbrotGLRenderer::MandelbrotGLRenderer(int w, int h)
    : window(nullptr), infoWindow(nullptr), width(w), height(h), 
      centerX(-0.5), centerY(0.0), zoom(1.0), maxIterations(100),
      isJuliaMode(false), juliaReal(-0.4), juliaImag(0.6),
      cursorRealPos(0.0), cursorImagPos(0.0), cursorInWindow(false),
      textureId(0), pixelBufferWidth(0), pixelBufferHeight(0),
      needsRecalculation(true), isCalculating(false),
      isDragging(false), lastMouseX(0.0), lastMouseY(0.0),
      gotoMode(false),
      lastFrameTime(0.0), fps(0.0), frameCount(0), fpsUpdateTime(0.0) {
    instance = this;
}

MandelbrotGLRenderer::~MandelbrotGLRenderer() {
    if (infoWindow) {
        delete infoWindow;
        infoWindow = nullptr;
    }
}

int MandelbrotGLRenderer::calculateIterationsForZoom(double zoomLevel) {
    // Base iterations + logarithmic scaling with zoom
    // This provides more detail as you zoom deeper
    int iterations = 100 + static_cast<int>(150 * std::log10(zoomLevel + 1));
    
    // Clamp between reasonable limits
    if (iterations < 5) iterations = 5;
    if (iterations > 10000) iterations = 10000;
    
    return iterations;
}

void MandelbrotGLRenderer::screenToComplex(double xpos, double ypos, double& realPos, double& imagPos) {
    double aspect = static_cast<double>(width) / height;
    double rangeY = 2.0 / zoom;
    double rangeX = rangeY * aspect;
    
    // Flip Y coordinate: GLFW has Y=0 at top, but we want Y increasing upward
    double flippedY = height - ypos;
    realPos = centerX + (xpos - width / 2.0) * rangeX / width;
    imagPos = centerY + (flippedY - height / 2.0) * rangeY / height;
}

void MandelbrotGLRenderer::clampIterations() {
    if (maxIterations < 5) maxIterations = 5;
    if (maxIterations > 10000) maxIterations = 10000;
}

void MandelbrotGLRenderer::calculateFrame() {
    isCalculating = true;
    
    // Make local copies to prevent race conditions during resize
    int localWidth = width;
    int localHeight = height;
    
    if (localWidth <= 0 || localHeight <= 0) {
        isCalculating = false;
        return;
    }

    pixels.resize(localWidth * localHeight * 3);
    
    double aspect = static_cast<double>(localWidth) / localHeight;
    double rangeY = 2.0 / zoom;
    double rangeX = rangeY * aspect;
    
    // Determine number of threads
    unsigned int numThreads = std::thread::hardware_concurrency();
    if (numThreads == 0) numThreads = 4; // Fallback if detection fails
    
    std::vector<std::thread> threads;
    
    // Total number of pixels
    int totalPixels = localWidth * localHeight;
    
    // Lambda function for each thread to calculate a range of pixels
    auto calculatePixels = [&](int startPixel, int endPixel) {
        for (int pixel = startPixel; pixel < endPixel; pixel++) {
            int y = pixel / localWidth;
            int x = pixel % localWidth;

            double real = centerX + (x - localWidth / 2.0) * rangeX / localWidth;
            double imag = centerY + (y - localHeight / 2.0) * rangeY / localHeight;

            int iterations;
            if (isJuliaMode) {
                iterations = MandelbrotCalculator::calculateJulia(real, imag, juliaReal, juliaImag, maxIterations);
            } else {
                iterations = MandelbrotCalculator::calculateMandelbrot(real, imag, maxIterations);
            }

            float r, g, b;
            MandelbrotCalculator::getRGB(iterations, maxIterations, r, g, b);

            int idx = pixel * 3;
            pixels[idx] = static_cast<unsigned char>(r * 255);
            pixels[idx + 1] = static_cast<unsigned char>(g * 255);
            pixels[idx + 2] = static_cast<unsigned char>(b * 255);
        }
    };

    // Divide pixels among threads
    int pixelsPerThread = totalPixels / numThreads;
    int remainderPixels = totalPixels % numThreads;
    
    int currentPixel = 0;
    for (unsigned int i = 0; i < numThreads; i++) {
        int startPixel = currentPixel;
        int endPixel = startPixel + pixelsPerThread + (i < static_cast<unsigned int>(remainderPixels) ? 1 : 0);
        
        threads.emplace_back(calculatePixels, startPixel, endPixel);
        currentPixel = endPixel;
    }
    
    // Wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }

    // Store the dimensions this buffer was calculated for
    pixelBufferWidth = localWidth;
    pixelBufferHeight = localHeight;
    
    isCalculating = false;
}

void MandelbrotGLRenderer::renderFrame() {
    // Don't render if currently calculating
    if (isCalculating) return;
    
    if (pixelBufferWidth <= 0 || pixelBufferHeight <= 0 || pixels.empty()) return;
    
    // Verify buffer size matches dimensions
    if (pixels.size() != static_cast<size_t>(pixelBufferWidth * pixelBufferHeight * 3)) return;
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Upload pixel data to texture using the dimensions the buffer was calculated for
    glBindTexture(GL_TEXTURE_2D, textureId);
    
    // Set pixel alignment to 1 byte to handle non-aligned widths
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, pixelBufferWidth, pixelBufferHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
    
    // Render texture as a fullscreen quad
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f, -1.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex2f( 1.0f, -1.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex2f( 1.0f,  1.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f,  1.0f);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    
    glfwSwapBuffers(window);
}

void MandelbrotGLRenderer::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        instance->handleKey(key);
    }
}

void MandelbrotGLRenderer::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    instance->handleResize(width, height);
}

void MandelbrotGLRenderer::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    instance->handleScroll(yoffset);
}

void MandelbrotGLRenderer::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    instance->handleMouseButton(button, action);
}

void MandelbrotGLRenderer::cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    instance->handleCursorPos(xpos, ypos);
}

void MandelbrotGLRenderer::cursorEnterCallback(GLFWwindow* window, int entered) {
    if (!entered) {
        instance->cursorInWindow = false;
    }
}

void MandelbrotGLRenderer::handleKey(int key) {
    bool needsUpdate = true;
    
    switch(key) {
        case GLFW_KEY_ESCAPE:
        case GLFW_KEY_Q:
            glfwSetWindowShouldClose(window, GLFW_TRUE);
            needsUpdate = false;
            break;
        case GLFW_KEY_LEFT:
        case GLFW_KEY_A:
            centerX -= 0.1 / zoom;
            break;
        case GLFW_KEY_RIGHT:
        case GLFW_KEY_D:
            centerX += 0.1 / zoom;
            break;
        case GLFW_KEY_UP:
        case GLFW_KEY_W:
            centerY -= 0.1 / zoom;
            break;
        case GLFW_KEY_DOWN:
        case GLFW_KEY_S:
            centerY += 0.1 / zoom;
            break;
        case GLFW_KEY_EQUAL:
        case GLFW_KEY_KP_ADD:
            maxIterations += 5;
            clampIterations();
            break;
        case GLFW_KEY_MINUS:
        case GLFW_KEY_KP_SUBTRACT:
            maxIterations -= 5;
            clampIterations();
            break;
        case GLFW_KEY_COMMA:
            maxIterations += 1000;
            clampIterations();
            break;
        case GLFW_KEY_PERIOD:
            maxIterations -= 1000;
            clampIterations();
            break;
        case GLFW_KEY_V:
            isJuliaMode = !isJuliaMode;
            if (isJuliaMode) {
                // When switching to Julia, use current cursor position as Julia constant
                if (cursorInWindow) {
                    juliaReal = cursorRealPos;
                    juliaImag = cursorImagPos;
                }
            }
            break;
        case GLFW_KEY_0:
        case GLFW_KEY_KP_0:
            // Reset to initial position and zoom
            centerX = -0.5;
            centerY = 0.0;
            zoom = 1.0;
            maxIterations = calculateIterationsForZoom(zoom);
            break;
        default:
            needsUpdate = false;
            break;
    }
    
    if (needsUpdate) {
        needsRecalculation = true;
    }
}

void MandelbrotGLRenderer::handleResize(int newWidth, int newHeight) {
    if (newWidth == 0 || newHeight == 0) return; // Skip invalid sizes
    
    width = newWidth;
    height = newHeight;
    
    glViewport(0, 0, width, height);
    
    // Mark for recalculation but don't do it immediately
    needsRecalculation = true;
}

void MandelbrotGLRenderer::handleScroll(double yoffset) {
    // Get cursor position and window dimensions
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    
    // Use local copies to ensure consistency
    int localWidth = width;
    int localHeight = height;
    double localZoom = zoom;
    
    if (localWidth <= 0 || localHeight <= 0) return;
    
    // Calculate complex coordinates at cursor position before zoom
    double cursorReal, cursorImag;
    screenToComplex(mouseX, mouseY, cursorReal, cursorImag);
    
    // Calculate aspect ratio for recentering after zoom
    double aspect = static_cast<double>(localWidth) / localHeight;
    double flippedMouseY = localHeight - mouseY;
    
    // Apply zoom
    if (yoffset > 0) {
        // Scroll up - zoom in
        localZoom *= 1.2;
    } else if (yoffset < 0) {
        // Scroll down - zoom out
        localZoom /= 1.2;
        if (localZoom < 1.0) localZoom = 1.0;
    }
    
    // Automatically adjust iterations based on zoom level
    maxIterations = calculateIterationsForZoom(localZoom);
    
    // Calculate new ranges with updated zoom
    double newRangeY = 2.0 / localZoom;
    double newRangeX = newRangeY * aspect;
    
    // Adjust center so the cursor position remains at the same complex coordinates
    centerX = cursorReal - (mouseX - localWidth / 2.0) * newRangeX / localWidth;
    centerY = cursorImag - (flippedMouseY - localHeight / 2.0) * newRangeY / localHeight;
    
    // Update zoom
    zoom = localZoom;
    
    needsRecalculation = true;
}

void MandelbrotGLRenderer::handleMouseButton(int button, int action) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            isDragging = true;
            glfwGetCursorPos(window, &lastMouseX, &lastMouseY);
        } else if (action == GLFW_RELEASE) {
            isDragging = false;
        }
    } else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        gotoMode = true;
    }
}

void MandelbrotGLRenderer::handleCursorPos(double xpos, double ypos) {
    // Update cursor position in complex plane
    screenToComplex(xpos, ypos, cursorRealPos, cursorImagPos);
    cursorInWindow = true;
    
    if (isDragging) {
        double deltaX = xpos - lastMouseX;
        double deltaY = ypos - lastMouseY;
        
        // Convert pixel movement to complex plane movement
        double aspect = static_cast<double>(width) / height;
        double rangeY = 2.0 / zoom;
        double rangeX = rangeY * aspect;
        centerX -= deltaX * rangeX / width;
        centerY += deltaY * rangeY / height;  // Invert Y for natural mouse drag
        
        lastMouseX = xpos;
        lastMouseY = ypos;
        
        needsRecalculation = true;
    }
}

void MandelbrotGLRenderer::handleGotoCoordinate() {
    std::cout << "\n=== Go To Coordinate ===";
    std::cout << "\nCurrent position: (" << centerX << ", " << centerY << ")";
    std::cout << "\nCurrent zoom: " << zoom << "x";
    std::cout << "\nCurrent iterations: " << maxIterations;
    std::cout << "\n\nEnter new values (or press Enter to keep current):";
    
    std::string input;
    double newCenterX = centerX;
    double newCenterY = centerY;
    double newZoom = zoom;
    int newIterations = maxIterations;
    
    std::cout << "\nReal coordinate (current: " << centerX << "): ";
    std::getline(std::cin, input);
    if (!input.empty()) {
        try {
            newCenterX = std::stod(input);
        } catch (...) {
            std::cout << "Invalid input, keeping current value.\n";
        }
    }
    
    std::cout << "Imaginary coordinate (current: " << centerY << "): ";
    std::getline(std::cin, input);
    if (!input.empty()) {
        try {
            newCenterY = std::stod(input);
        } catch (...) {
            std::cout << "Invalid input, keeping current value.\n";
        }
    }
    
    std::cout << "Zoom level (current: " << zoom << "): ";
    std::getline(std::cin, input);
    if (!input.empty()) {
        try {
            newZoom = std::stod(input);
            if (newZoom < 1.0) newZoom = 1.0;
        } catch (...) {
            std::cout << "Invalid input, keeping current value.\n";
        }
    }
    
    std::cout << "Iterations (current: " << maxIterations << "): ";
    std::getline(std::cin, input);
    if (!input.empty()) {
        try {
            newIterations = std::stoi(input);
        } catch (...) {
            std::cout << "Invalid input, keeping current value.\n";
        }
    }
    
    // Apply the new values
    centerX = newCenterX;
    centerY = newCenterY;
    zoom = newZoom;
    maxIterations = newIterations;
    clampIterations();
    
    std::cout << "\nJumping to: (" << centerX << ", " << centerY << ") ";
    std::cout << "at zoom " << zoom << "x with " << maxIterations << " iterations\n\n";
    
    needsRecalculation = true;
    gotoMode = false;
}

void MandelbrotGLRenderer::updateTitle() {
    char title[256];
    if (isJuliaMode) {
        snprintf(title, sizeof(title), 
                 "Julia Set (c=%.3f%+.3fi) - Center: (%.6f, %.6f) | Zoom: %.2fx | Iterations: %d | FPS: %.1f",
                 juliaReal, juliaImag, centerX, centerY, zoom, maxIterations, fps);
    } else {
        snprintf(title, sizeof(title), 
                 "Mandelbrot - Center: (%.6f, %.6f) | Zoom: %.2fx | Iterations: %d | FPS: %.1f",
                 centerX, centerY, zoom, maxIterations, fps);
    }
    glfwSetWindowTitle(window, title);
}

bool MandelbrotGLRenderer::init() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return false;
    }
    
    window = glfwCreateWindow(width, height, "Mandelbrot", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return false;
    }
    
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetCursorEnterCallback(window, cursorEnterCallback);
    
    // Create and configure texture
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glViewport(0, 0, width, height);
    
    // Create info window
    infoWindow = new InfoWindow();
    int mainX, mainY;
    glfwGetWindowPos(window, &mainX, &mainY);
    if (!infoWindow->init(mainX, mainY, width)) {
        std::cerr << "Failed to initialize info window\n";
        delete infoWindow;
        infoWindow = nullptr;
    }
    
    // Set context back to main window
    glfwMakeContextCurrent(window);
    
    updateTitle();
    return true;
}

void MandelbrotGLRenderer::run() {
    lastFrameTime = glfwGetTime();
    fpsUpdateTime = lastFrameTime;
    
    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        frameCount++;
        
        // Update FPS every 0.5 seconds
        if (currentTime - fpsUpdateTime >= 0.5) {
            fps = frameCount / (currentTime - fpsUpdateTime);
            frameCount = 0;
            fpsUpdateTime = currentTime;
            updateTitle();
        }
        
        // Handle goto coordinate mode
        if (gotoMode) {
            handleGotoCoordinate();
        }
        
        if (needsRecalculation && !isCalculating) {
            calculateFrame();
            needsRecalculation = false;
        }
        
        // Always render current buffer (even if old dimensions during resize)
        if (!isCalculating) {
            renderFrame();
            updateTitle();
        }
        
        // Update and render info window
        if (infoWindow) {
            infoWindow->update(cursorRealPos, cursorImagPos, cursorInWindow, zoom, maxIterations);
            infoWindow->render();
            glfwMakeContextCurrent(window);
        }
        
        glfwPollEvents();
        
        // Small sleep to avoid busy waiting
        if (!needsRecalculation) {
            glfwWaitEventsTimeout(0.016); // ~60 FPS
        }
        
        lastFrameTime = currentTime;
    }
    
    glDeleteTextures(1, &textureId);
    glfwDestroyWindow(window);
    glfwTerminate();
}
