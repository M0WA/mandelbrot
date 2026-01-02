#include "catch.hpp"
#include "../mandelbrot_renderer.h"

// Mock renderer for testing helper methods
class MandelbrotRendererTestable : public MandelbrotGLRenderer {
public:
    MandelbrotRendererTestable() : MandelbrotGLRenderer(800, 600) {
        // Initialize without GLFW
    }
    
    // Expose protected methods for testing
    using MandelbrotGLRenderer::calculateIterationsForZoom;
    using MandelbrotGLRenderer::screenToComplex;
    using MandelbrotGLRenderer::clampIterations;
    
    // Setters for testing coordinate transformation
    void setDimensions(int w, int h) {
        width = w;
        height = h;
    }
    
    void setCenter(double cx, double cy) {
        centerX = cx;
        centerY = cy;
    }
    
    void setZoom(double z) {
        zoom = z;
    }
    
    void setMaxIterations(int iter) {
        maxIterations = iter;
    }
    
    int getMaxIterations() const {
        return maxIterations;
    }
    
    // We need to make these members accessible for testing
    using MandelbrotGLRenderer::width;
    using MandelbrotGLRenderer::height;
    using MandelbrotGLRenderer::centerX;
    using MandelbrotGLRenderer::centerY;
    using MandelbrotGLRenderer::zoom;
    using MandelbrotGLRenderer::maxIterations;
};

TEST_CASE("MandelbrotGLRenderer::calculateIterationsForZoom", "[renderer]") {
    MandelbrotRendererTestable renderer;
    
    SECTION("Zoom level 1.0 should return base iterations") {
        int iterations = renderer.calculateIterationsForZoom(1.0);
        REQUIRE(iterations >= 100);
        REQUIRE(iterations <= 10000);
    }
    
    SECTION("Higher zoom should increase iterations") {
        int iter1 = renderer.calculateIterationsForZoom(1.0);
        int iter2 = renderer.calculateIterationsForZoom(100.0);
        REQUIRE(iter2 > iter1);
    }
    
    SECTION("Iterations should be clamped to minimum 5") {
        int iterations = renderer.calculateIterationsForZoom(0.0);
        REQUIRE(iterations >= 5);
    }
    
    SECTION("Iterations should be clamped to maximum 10000") {
        int iterations = renderer.calculateIterationsForZoom(1e10);
        REQUIRE(iterations <= 10000);
    }
    
    SECTION("Very high zoom should max out iterations") {
        int iterations = renderer.calculateIterationsForZoom(1e100);
        REQUIRE(iterations == 10000);
    }
}

TEST_CASE("MandelbrotGLRenderer::clampIterations", "[renderer]") {
    MandelbrotRendererTestable renderer;
    
    SECTION("Clamp below minimum") {
        renderer.setMaxIterations(0);
        renderer.clampIterations();
        REQUIRE(renderer.getMaxIterations() == 5);
    }
    
    SECTION("Clamp above maximum") {
        renderer.setMaxIterations(20000);
        renderer.clampIterations();
        REQUIRE(renderer.getMaxIterations() == 10000);
    }
    
    SECTION("Valid value should remain unchanged") {
        renderer.setMaxIterations(500);
        renderer.clampIterations();
        REQUIRE(renderer.getMaxIterations() == 500);
    }
    
    SECTION("Minimum boundary") {
        renderer.setMaxIterations(5);
        renderer.clampIterations();
        REQUIRE(renderer.getMaxIterations() == 5);
    }
    
    SECTION("Maximum boundary") {
        renderer.setMaxIterations(10000);
        renderer.clampIterations();
        REQUIRE(renderer.getMaxIterations() == 10000);
    }
}

TEST_CASE("MandelbrotGLRenderer::screenToComplex", "[renderer]") {
    MandelbrotRendererTestable renderer;
    renderer.setDimensions(800, 600);
    renderer.setCenter(0.0, 0.0);
    renderer.setZoom(1.0);
    
    double realPos, imagPos;
    
    SECTION("Center of screen maps to center of complex plane") {
        renderer.screenToComplex(400, 300, realPos, imagPos);
        REQUIRE(std::abs(realPos - 0.0) < 0.01);
        REQUIRE(std::abs(imagPos - 0.0) < 0.01);
    }
    
    SECTION("Top-left corner") {
        renderer.screenToComplex(0, 0, realPos, imagPos);
        // With zoom 1.0, aspect 800/600 = 4/3
        // RangeY = 2.0, RangeX = 2.0 * 4/3 = 8/3
        // realPos should be negative, imagPos should be positive
        REQUIRE(realPos < 0.0);
        REQUIRE(imagPos > 0.0);
    }
    
    SECTION("Bottom-right corner") {
        renderer.screenToComplex(800, 600, realPos, imagPos);
        REQUIRE(realPos > 0.0);
        REQUIRE(imagPos < 0.0);
    }
    
    SECTION("Zooming in should scale coordinates") {
        double real1, imag1, real2, imag2;
        
        renderer.setZoom(1.0);
        renderer.screenToComplex(600, 300, real1, imag1);
        
        renderer.setZoom(2.0);
        renderer.screenToComplex(600, 300, real2, imag2);
        
        // At 2x zoom, same screen position should be closer to center
        REQUIRE(std::abs(real2) < std::abs(real1));
    }
    
    SECTION("Changing center should offset coordinates") {
        double real1, imag1, real2, imag2;
        
        renderer.setCenter(0.0, 0.0);
        renderer.screenToComplex(400, 300, real1, imag1);
        
        renderer.setCenter(1.0, 1.0);
        renderer.screenToComplex(400, 300, real2, imag2);
        
        REQUIRE(std::abs(real2 - 1.0) < 0.01);
        REQUIRE(std::abs(imag2 - 1.0) < 0.01);
    }
}

TEST_CASE("MandelbrotGLRenderer coordinate system consistency", "[renderer]") {
    MandelbrotRendererTestable renderer;
    renderer.setDimensions(800, 600);
    renderer.setCenter(-0.5, 0.0);
    renderer.setZoom(1.0);
    
    SECTION("Y-axis flip is consistent") {
        double real1, imag1, real2, imag2;
        
        // Top of screen (y=0) should have positive imaginary
        renderer.screenToComplex(400, 0, real1, imag1);
        
        // Bottom of screen (y=600) should have negative imaginary
        renderer.screenToComplex(400, 600, real2, imag2);
        
        REQUIRE(imag1 > imag2);
    }
    
    SECTION("Aspect ratio preservation") {
        renderer.setDimensions(1600, 800);  // 2:1 aspect ratio
        renderer.setCenter(0.0, 0.0);
        renderer.setZoom(1.0);
        
        double realLeft, imagLeft, realRight, imagRight;
        
        renderer.screenToComplex(0, 400, realLeft, imagLeft);
        renderer.screenToComplex(1600, 400, realRight, imagRight);
        
        double widthComplex = realRight - realLeft;
        
        // With aspect 2:1, width should be 2x the height (which is 2.0 at zoom 1.0)
        REQUIRE(std::abs(widthComplex - 4.0) < 0.1);
    }
}
