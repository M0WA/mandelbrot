#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../mandelbrot_calculator.h"

TEST_CASE("MandelbrotCalculator::calculateMandelbrot", "[mandelbrot]") {
    SECTION("Point at origin should iterate to max") {
        int iterations = MandelbrotCalculator::calculateMandelbrot(0.0, 0.0, 100);
        REQUIRE(iterations == 100);
    }
    
    SECTION("Point at (2, 2) should diverge quickly") {
        int iterations = MandelbrotCalculator::calculateMandelbrot(2.0, 2.0, 100);
        REQUIRE(iterations < 10);
    }
    
    SECTION("Point at (-2, 0) should be in the set") {
        int iterations = MandelbrotCalculator::calculateMandelbrot(-2.0, 0.0, 100);
        REQUIRE(iterations == 100);
    }
    
    SECTION("Point clearly outside set diverges immediately") {
        int iterations = MandelbrotCalculator::calculateMandelbrot(10.0, 10.0, 100);
        REQUIRE(iterations == 1);
    }
    
    SECTION("Classic Mandelbrot point c = -0.5") {
        int iterations = MandelbrotCalculator::calculateMandelbrot(-0.5, 0.0, 100);
        REQUIRE(iterations == 100);
    }
}

TEST_CASE("MandelbrotCalculator::calculateJulia", "[julia]") {
    SECTION("Julia set with c = 0 is a circle of radius 2") {
        // Points inside circle should iterate to max
        int iterations = MandelbrotCalculator::calculateJulia(0.5, 0.5, 0.0, 0.0, 100);
        REQUIRE(iterations == 100);
    }
    
    SECTION("Julia set point outside") {
        int iterations = MandelbrotCalculator::calculateJulia(5.0, 5.0, -0.4, 0.6, 100);
        REQUIRE(iterations < 10);
    }
    
    SECTION("Julia set with c = -0.4 + 0.6i at origin") {
        int iterations = MandelbrotCalculator::calculateJulia(0.0, 0.0, -0.4, 0.6, 100);
        REQUIRE(iterations > 0);
        REQUIRE(iterations < 100);
    }
}

TEST_CASE("MandelbrotCalculator::getRGB", "[colors]") {
    float r, g, b;
    
    SECTION("Max iterations should be black") {
        MandelbrotCalculator::getRGB(100, 100, r, g, b);
        REQUIRE(r == 0.0f);
        REQUIRE(g == 0.0f);
        REQUIRE(b == 0.0f);
    }
    
    SECTION("Zero iterations should have non-zero blue component") {
        MandelbrotCalculator::getRGB(0, 100, r, g, b);
        REQUIRE(b > 0.0f);
    }
    
    SECTION("RGB values should be in valid range [0, 1]") {
        for (int i = 0; i < 100; i++) {
            MandelbrotCalculator::getRGB(i, 100, r, g, b);
            REQUIRE(r >= 0.0f);
            REQUIRE(r <= 1.0f);
            REQUIRE(g >= 0.0f);
            REQUIRE(g <= 1.0f);
            REQUIRE(b >= 0.0f);
            REQUIRE(b <= 1.0f);
        }
    }
    
    SECTION("Different iteration counts should produce different colors") {
        float r1, g1, b1, r2, g2, b2;
        MandelbrotCalculator::getRGB(10, 100, r1, g1, b1);
        MandelbrotCalculator::getRGB(50, 100, r2, g2, b2);
        
        bool different = (r1 != r2) || (g1 != g2) || (b1 != b2);
        REQUIRE(different);
    }
}

TEST_CASE("MandelbrotCalculator edge cases", "[edge-cases]") {
    SECTION("Zero max iterations should return 0") {
        int iterations = MandelbrotCalculator::calculateMandelbrot(0.5, 0.5, 0);
        REQUIRE(iterations == 0);
    }
    
    SECTION("Single iteration") {
        int iterations = MandelbrotCalculator::calculateMandelbrot(0.5, 0.5, 1);
        REQUIRE(iterations <= 1);
    }
    
    SECTION("Very high iteration count") {
        int iterations = MandelbrotCalculator::calculateMandelbrot(0.0, 0.0, 10000);
        REQUIRE(iterations == 10000);
    }
}
