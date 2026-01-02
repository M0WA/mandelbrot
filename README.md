# Mandelbrot Set Renderer

An interactive C++ OpenGL application that renders the Mandelbrot set with real-time navigation and zooming.

## Features

- Real-time Mandelbrot set rendering in high-resolution OpenGL window
- Interactive navigation with arrow keys and mouse
- Zoom in/out with mouse wheel
- Click and drag to pan
- Automatic iteration adjustment based on zoom level
- Smooth RGB color gradients
- FPS counter and performance monitoring

## Controls

- **Arrow Keys** (↑↓←→) / **W/A/S/D**: Move the viewport
- **Mouse Wheel**: Zoom in/out (centers on cursor position)
- **Left Mouse Button + Drag**: Pan the viewport
- **+ / =**: Increase iterations by 5
- **- / _**: Decrease iterations by 5
- **0**: Reset to initial view
- **Q / ESC**: Quit

## Building

Requires GLFW and OpenGL libraries:
```bash
sudo apt-get install libglfw3-dev libgl1-mesa-dev  # On Ubuntu/Debian
make
```

Or manually:
```bash
g++ -std=c++11 -O2 -Wall -c mandelbrot_gl.cpp -o build/mandelbrot_gl.o
g++ -std=c++11 -O2 -Wall -c mandelbrot_calculator.cpp -o build/mandelbrot_calculator.o
g++ -std=c++11 -O2 -Wall -o mandelbrot_gl build/mandelbrot_gl.o build/mandelbrot_calculator.o -lglfw -lGL
```

## Running

```bash
./mandelbrot_gl
```

Or:
```bash
make run
```

## How It Works

The application calculates the Mandelbrot set for each pixel position. Each point is colored based on how many iterations it takes to escape (or if it stays bounded). The color gradient goes from blue → cyan → green → yellow → red → magenta.

**Automatic iteration scaling**: Iterations automatically increase logarithmically as you zoom deeper, providing appropriate detail at each zoom level while maintaining performance.

## Architecture

- **mandelbrot_calculator.cpp/h** - Core Mandelbrot set calculation and color mapping
- **mandelbrot_gl.cpp** - OpenGL rendering, window management, and user input handling
- **build/** - Compiled object files (automatically created)

## Requirements

- Linux/Unix system
- g++ compiler with C++11 support
- GLFW3 library
- OpenGL library
