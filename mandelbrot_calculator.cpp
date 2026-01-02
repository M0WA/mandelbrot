#include "mandelbrot_calculator.h"
#include <complex>

int MandelbrotCalculator::calculateMandelbrot(double real, double imag, int maxIterations) {
    std::complex<double> c(real, imag);
    std::complex<double> z(0, 0);
    
    int iterations = 0;
    while (std::abs(z) <= 2.0 && iterations < maxIterations) {
        z = z * z + c;
        iterations++;
    }
    
    return iterations;
}

int MandelbrotCalculator::calculateJulia(double real, double imag, double cReal, double cImag, int maxIterations) {
    std::complex<double> z(real, imag);
    std::complex<double> c(cReal, cImag);
    
    int iterations = 0;
    while (iterations < maxIterations && std::abs(z) <= 2.0) {
        z = z * z + c;
        iterations++;
    }
    
    return iterations;
}

void MandelbrotCalculator::getRGB(int iterations, int maxIterations, float& r, float& g, float& b) {
    if (iterations == maxIterations) {
        r = g = b = 0.0f;
        return;
    }

    float t = static_cast<float>(iterations) / maxIterations;
    
    // Color gradient: blue -> cyan -> green -> yellow -> red -> magenta
    if (t < 0.16f) {
        r = 0.0f; g = 0.0f; b = 0.5f + t * 3.0f;
    } else if (t < 0.33f) {
        r = 0.0f; g = (t - 0.16f) * 6.0f; b = 1.0f;
    } else if (t < 0.5f) {
        r = 0.0f; g = 1.0f; b = 1.0f - (t - 0.33f) * 6.0f;
    } else if (t < 0.66f) {
        r = (t - 0.5f) * 6.0f; g = 1.0f; b = 0.0f;
    } else if (t < 0.83f) {
        r = 1.0f; g = 1.0f - (t - 0.66f) * 6.0f; b = 0.0f;
    } else {
        r = 1.0f; g = 0.0f; b = (t - 0.83f) * 6.0f;
    }
}
