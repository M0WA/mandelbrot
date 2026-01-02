#ifndef MANDELBROT_CALCULATOR_H
#define MANDELBROT_CALCULATOR_H

class MandelbrotCalculator {
public:
    // Calculate the number of iterations for a point in the complex plane
    static int calculateMandelbrot(double real, double imag, int maxIterations);
    
    // Calculate Julia set iterations
    static int calculateJulia(double real, double imag, double cReal, double cImag, int maxIterations);
    
    // Convert iteration count to RGB color values (0.0 - 1.0)
    static void getRGB(int iterations, int maxIterations, float& r, float& g, float& b);
};

#endif // MANDELBROT_CALCULATOR_H
