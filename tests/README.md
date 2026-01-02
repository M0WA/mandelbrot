# Test Suite

This directory contains the test suite for the Mandelbrot renderer project.

## Running Tests

```bash
make test
```

## Test Structure

### test_mandelbrot_calculator.cpp
Tests for the core calculation functions:
- **calculateMandelbrot**: Tests various points inside/outside the Mandelbrot set
- **calculateJulia**: Tests Julia set calculations with different constants
- **getRGB**: Tests color mapping for different iteration counts
- **Edge cases**: Tests boundary conditions and extreme values

### test_mandelbrot_renderer.cpp
Tests for renderer helper methods:
- **calculateIterationsForZoom**: Tests automatic iteration scaling based on zoom level
- **clampIterations**: Tests iteration value clamping [5, 10000]
- **screenToComplex**: Tests screen-to-complex coordinate transformation
- **Coordinate system consistency**: Tests Y-axis flipping and aspect ratio preservation

## Test Framework

The tests use a custom lightweight testing framework (`catch.hpp`) inspired by Catch2, providing:
- `TEST_CASE(name, tags)` - Define test cases
- `SECTION(name)` - Group related assertions
- `REQUIRE(expression)` - Assert conditions

## Test Coverage

The test suite covers:
- ✓ Mandelbrot set calculations
- ✓ Julia set calculations  
- ✓ Color mapping (RGB generation)
- ✓ Iteration management (clamping, auto-scaling)
- ✓ Coordinate transformations (screen ↔ complex plane)
- ✓ Edge cases and boundary conditions
- ✓ Aspect ratio handling
- ✓ Y-axis coordinate flipping

## Adding New Tests

To add new tests:

1. Create or edit test files in `tests/` directory
2. Use the TEST_CASE macro:
   ```cpp
   TEST_CASE("Description of test", "[tag]") {
       SECTION("Specific scenario") {
           REQUIRE(condition == expected);
       }
   }
   ```
3. Run `make test` to verify

## Implementation Notes

- The renderer class uses `protected` access for testability
- Tests compile with `-DTESTING` flag
- Tests do not require GLFW initialization (testing logic only)
- A separate test-specific renderer object is built without GUI dependencies
