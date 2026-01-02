#include "info_window.h"
#include <iostream>
#include <vector>
#include <cstdio>

InfoWindow::InfoWindow()
    : window(nullptr), ftLibrary(nullptr), ftFace(nullptr),
      cursorRealPos(0.0), cursorImagPos(0.0), cursorValid(false),
      zoomLevel(1.0), maxIterations(100) {
}

InfoWindow::~InfoWindow() {
    cleanup();
}

bool InfoWindow::init(int mainWindowX, int mainWindowY, int mainWindowWidth) {
    // Create info window
    window = glfwCreateWindow(400, 120, "Mandelbrot Info", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create info window\n";
        return false;
    }
    
    // Position info window next to main window
    glfwSetWindowPos(window, mainWindowX + mainWindowWidth + 10, mainWindowY);
    
    // Initialize FreeType for text rendering
    if (!initFreeType()) {
        glfwDestroyWindow(window);
        window = nullptr;
        return false;
    }
    
    return true;
}

bool InfoWindow::initFreeType() {
    if (FT_Init_FreeType(&ftLibrary)) {
        std::cerr << "Failed to initialize FreeType\n";
        return false;
    }
    
    // Load a system font
    if (FT_New_Face(ftLibrary, "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 0, &ftFace)) {
        std::cerr << "Failed to load font\n";
        return false;
    }
    
    FT_Set_Pixel_Sizes(ftFace, 0, 48);
    
    glfwMakeContextCurrent(window);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    // Load first 128 ASCII characters
    for (unsigned char c = 0; c < 128; c++) {
        if (FT_Load_Char(ftFace, c, FT_LOAD_RENDER)) {
            std::cerr << "Failed to load glyph: " << c << "\n";
            continue;
        }
        
        // Convert grayscale bitmap to RGBA
        int width = ftFace->glyph->bitmap.width;
        int height = ftFace->glyph->bitmap.rows;
        std::vector<unsigned char> rgbaBuffer(width * height * 4);
        
        // Convert FreeType bitmap to RGBA (no Y-flip needed with our projection)
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                int srcIdx = i * width + j;
                int dstIdx = (i * width + j) * 4;
                unsigned char gray = ftFace->glyph->bitmap.buffer[srcIdx];
                
                // White RGB, grayscale alpha - this allows color tinting via glColor
                rgbaBuffer[dstIdx + 0] = 255;  // R
                rgbaBuffer[dstIdx + 1] = 255;  // G
                rgbaBuffer[dstIdx + 2] = 255;  // B
                rgbaBuffer[dstIdx + 3] = gray; // A (0=transparent background, 255=solid glyph)
            }
        }
        
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
                     0, GL_RGBA, GL_UNSIGNED_BYTE, rgbaBuffer.data());
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        
        Character character = {
            texture,
            width,
            height,
            ftFace->glyph->bitmap_left,
            ftFace->glyph->bitmap_top,
            static_cast<unsigned int>(ftFace->glyph->advance.x)
        };
        characters[c] = character;
    }
    
    return true;
}

void InfoWindow::update(double cursorReal, double cursorImag, bool cursorInWindow,
                       double zoom, int iterations) {
    cursorRealPos = cursorReal;
    cursorImagPos = cursorImag;
    cursorValid = cursorInWindow;
    zoomLevel = zoom;
    maxIterations = iterations;
}

void InfoWindow::render() {
    if (!window || glfwWindowShouldClose(window)) return;
    
    glfwMakeContextCurrent(window);
    
    int infoWidth, infoHeight;
    glfwGetWindowSize(window, &infoWidth, &infoHeight);
    
    glViewport(0, 0, infoWidth, infoHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, infoWidth, 0, infoHeight, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Render text fields
    float yPos = infoHeight - 30;
    
    // Cursor Position
    std::string cursorText;
    if (cursorValid) {
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "Cursor: (%.6f, %.6f)", cursorRealPos, cursorImagPos);
        cursorText = buffer;
    } else {
        cursorText = "Cursor: (N/A)";
    }
    renderText(cursorText, 10, yPos, 0.5f, 1.0f, 1.0f, 1.0f);
    yPos -= 30;
    
    // Zoom Level
    char zoomBuffer[128];
    snprintf(zoomBuffer, sizeof(zoomBuffer), "Zoom: %.2fx", zoomLevel);
    renderText(std::string(zoomBuffer), 10, yPos, 0.5f, 0.5f, 1.0f, 0.5f);
    yPos -= 30;
    
    // Iterations
    char iterBuffer[128];
    snprintf(iterBuffer, sizeof(iterBuffer), "Iterations: %d", maxIterations);
    renderText(std::string(iterBuffer), 10, yPos, 0.5f, 0.5f, 0.5f, 1.0f);
    
    glfwSwapBuffers(window);
}

void InfoWindow::renderText(const std::string& text, float x, float y, float scale,
                           float r, float g, float b) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    
    // Important: Set texture environment to modulate so glColor affects the texture
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    
    for (char c : text) {
        Character ch = characters[c];
        
        float xpos = x + ch.bearingX * scale;
        float ypos = y - (ch.sizeY - ch.bearingY) * scale;
        float w = ch.sizeX * scale;
        float h = ch.sizeY * scale;
        
        glBindTexture(GL_TEXTURE_2D, ch.textureID);
        glColor4f(r, g, b, 1.0f);  // Use glColor4f for proper alpha support
        
        glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 0.0f); glVertex2f(xpos, ypos + h);
            glTexCoord2f(1.0f, 0.0f); glVertex2f(xpos + w, ypos + h);
            glTexCoord2f(1.0f, 1.0f); glVertex2f(xpos + w, ypos);
            glTexCoord2f(0.0f, 1.0f); glVertex2f(xpos, ypos);
        glEnd();
        
        x += (ch.advance >> 6) * scale;
    }
    
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}

bool InfoWindow::shouldClose() {
    return window && glfwWindowShouldClose(window);
}

void InfoWindow::cleanup() {
    // Clean up FreeType character textures
    for (auto& pair : characters) {
        glDeleteTextures(1, &pair.second.textureID);
    }
    characters.clear();
    
    if (ftFace) {
        FT_Done_Face(ftFace);
        ftFace = nullptr;
    }
    
    if (ftLibrary) {
        FT_Done_FreeType(ftLibrary);
        ftLibrary = nullptr;
    }
    
    if (window) {
        glfwDestroyWindow(window);
        window = nullptr;
    }
}
