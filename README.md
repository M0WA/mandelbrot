# Mandelbrot and Julia Set Renderer

An interactive C++ OpenGL application that renders the Mandelbrot and Julia sets with real-time navigation and zooming.

## Features

- Real-time Mandelbrot and Julia set rendering in high-resolution OpenGL window
- Separate info window displaying cursor position, zoom level, and iterations
- Interactive navigation with arrow keys and mouse
- Cursor-centered zoom with mouse wheel
- Click and drag to pan
- Toggle between Mandelbrot and Julia sets
- Automatic iteration adjustment based on zoom level
- Multithreaded calculation for optimal performance
- Smooth RGB color gradients
- FPS counter and performance monitoring
- Text rendering using FreeType

## Controls

- **Arrow Keys** (↑↓←→) / **W/A/S/D**: Move the viewport
- **Mouse Wheel**: Zoom in/out (centers on cursor position)
- **Left Mouse Button + Drag**: Pan the viewport
- **Right Mouse Button**: Go to specific coordinate (console input)
- **+ / =**: Increase iterations by 5
- **- / _**: Decrease iterations by 5
- **, (comma)**: Increase iterations by 1000
- **. (period)**: Decrease iterations by 1000
- **V**: Toggle between Mandelbrot and Julia set (uses cursor position as Julia constant)
- **0**: Reset to initial view
- **Q / ESC**: Quit

## Building

### From Source

Requires GLFW, OpenGL, and FreeType libraries:
```bash
sudo apt-get install libglfw3-dev libgl1-mesa-dev libfreetype-dev  # On Ubuntu/Debian
make
```

### Debian Package

Build a Debian package:
```bash
make deb
```

This creates `mandelbrot-renderer_1.0.0_amd64.deb` in the parent directory.

Install the package:
```bash
sudo dpkg -i ../mandelbrot-renderer_1.0.0_amd64.deb
```

## Installation

### From Source
```bash
sudo make install
```

### From Debian Package
```bash
sudo dpkg -i mandelbrot-renderer_1.0.0_amd64.deb
```

## Running

```bash
./mandelbrot
```

Or if installed:
```bash
mandelbrot
```

Or:
```bash
make run
```

## Documentation

View the manual page:
```bash
man mandelbrot
```

## How It Works

The application calculates the Mandelbrot or Julia set for each pixel position using multithreaded computation. Each point is colored based on how many iterations it takes to escape (or if it stays bounded). The color gradient goes from blue → cyan → green → yellow → red → magenta.

**Automatic iteration scaling**: Iterations automatically increase logarithmically as you zoom deeper, providing appropriate detail at each zoom level while maintaining performance.

**Julia Set Mode**: When you press V, the application switches to Julia set mode using the current cursor position as the complex constant. This allows you to explore different Julia sets by positioning your cursor over interesting areas of the Mandelbrot set.

## Architecture

- **main.cpp** - Entry point and control instructions
- **mandelbrot_renderer.cpp/h** - Main renderer with dual windows and input handling
- **mandelbrot_calculator.cpp/h** - Core Mandelbrot and Julia set calculation and color mapping
- **info_window.cpp/h** - Info window with FreeType text rendering
- **man/mandelbrot.1** - Man page documentation
- **debian/** - Debian packaging files
- **build/** - Compiled object files (automatically created)

## Requirements

- Linux/Unix system
- g++ compiler with C++11 support
- GLFW3 library
- OpenGL library
- FreeType library

## Uninstallation

### From Source Installation
```bash
sudo make uninstall
```

### From Debian Package
```bash
sudo dpkg -r mandelbrot-renderer
```
