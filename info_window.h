#ifndef INFO_WINDOW_H
#define INFO_WINDOW_H

#include <GLFW/glfw3.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <string>
#include <map>

struct Character {
    GLuint textureID;
    int sizeX;
    int sizeY;
    int bearingX;
    int bearingY;
    unsigned int advance;
};

class InfoWindow {
public:
    InfoWindow();
    ~InfoWindow();
    
    bool init(int mainWindowX, int mainWindowY, int mainWindowWidth);
    void update(double cursorReal, double cursorImag, bool cursorInWindow, 
                double zoom, int iterations);
    void render();
    bool shouldClose();
    void cleanup();
    
private:
    bool initFreeType();
    void renderText(const std::string& text, float x, float y, float scale, 
                   float r, float g, float b);
    
    GLFWwindow* window;
    FT_Library ftLibrary;
    FT_Face ftFace;
    std::map<char, Character> characters;
    
    // Current state for rendering
    double cursorRealPos;
    double cursorImagPos;
    bool cursorValid;
    double zoomLevel;
    int maxIterations;
};

#endif
