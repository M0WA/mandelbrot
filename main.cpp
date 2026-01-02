#include <iostream>
#include "mandelbrot_renderer.h"

int main(int argc, char* argv[]) {
    MandelbrotGLRenderer renderer;
    
    if (!renderer.init()) {
        return 1;
    }
    
    std::cout << "Controls:\n"
              << "  Arrow keys / WASD: Move viewport\n"
              << "  Mouse wheel      : Zoom (cursor-centered)\n"
              << "  Left-click drag  : Pan viewport\n"
              << "  Right-click      : Go to coordinate\n"
              << "  +/-              : Increase/decrease iterations by 5\n"
              << "  ,/.              : Increase/decrease iterations by 1000\n"
              << "  V                : Toggle between Mandelbrot and Julia set\n"
              << "  0                : Reset to initial view\n"
              << "  Q / ESC          : Quit\n\n";
    
    renderer.run();
    return 0;
}
