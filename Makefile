CXX = g++
CXXFLAGS = -std=c++11 -O2 -Wall -I/usr/include/freetype2
BUILD_DIR = build
TARGET = mandelbrot
SRC = main.cpp mandelbrot_renderer.cpp mandelbrot_calculator.cpp info_window.cpp
OBJ = $(BUILD_DIR)/main.o $(BUILD_DIR)/mandelbrot_renderer.o $(BUILD_DIR)/mandelbrot_calculator.o $(BUILD_DIR)/info_window.o
LIBS = -lglfw -lGL -lpthread -lfreetype

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ) $(LIBS)

$(BUILD_DIR)/main.o: main.cpp mandelbrot_renderer.h | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c main.cpp -o $(BUILD_DIR)/main.o

$(BUILD_DIR)/mandelbrot_renderer.o: mandelbrot_renderer.cpp mandelbrot_renderer.h mandelbrot_calculator.h info_window.h | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c mandelbrot_renderer.cpp -o $(BUILD_DIR)/mandelbrot_renderer.o

$(BUILD_DIR)/mandelbrot_calculator.o: mandelbrot_calculator.cpp mandelbrot_calculator.h | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c mandelbrot_calculator.cpp -o $(BUILD_DIR)/mandelbrot_calculator.o

$(BUILD_DIR)/info_window.o: info_window.cpp info_window.h | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c info_window.cpp -o $(BUILD_DIR)/info_window.o

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -f $(TARGET)
	rm -rf $(BUILD_DIR)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
