#ifndef MANDELBROT_RENDERER_H
#define MANDELBROT_RENDERER_H

#include <GLFW/glfw3.h>
#include <vector>
#include <string>

class InfoWindow;

class MandelbrotGLRenderer {
private:
    GLFWwindow* window;
    InfoWindow* infoWindow;
    int width, height;
    double centerX, centerY;
    double zoom;
    int maxIterations;
    bool isJuliaMode;
    double juliaReal, juliaImag;
    
    double cursorRealPos, cursorImagPos;
    bool cursorInWindow;
    
    std::vector<unsigned char> pixels;
    GLuint textureId;
    int pixelBufferWidth, pixelBufferHeight;
    
    bool needsRecalculation;
    bool isCalculating;
    
    bool isDragging;
    double lastMouseX, lastMouseY;
    
    bool gotoMode;
    
    double lastFrameTime;
    double fps;
    int frameCount;
    double fpsUpdateTime;
    
    static MandelbrotGLRenderer* instance;
    
    int calculateIterationsForZoom(double zoomLevel);
    void calculateFrame();
    void renderFrame();
    
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    static void cursorEnterCallback(GLFWwindow* window, int entered);
    
    void handleKey(int key);
    void handleResize(int newWidth, int newHeight);
    void handleScroll(double yoffset);
    void handleMouseButton(int button, int action);
    void handleCursorPos(double xpos, double ypos);
    void handleGotoCoordinate();
    void updateTitle();
    
public:
    MandelbrotGLRenderer(int w = 800, int h = 600);
    ~MandelbrotGLRenderer();
    bool init();
    void run();
};

#endif // MANDELBROT_RENDERER_H
