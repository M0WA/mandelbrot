CXX = g++
CXXFLAGS = -std=c++11 -O2 -Wall -I/usr/include/freetype2
BUILD_DIR = build
TEST_DIR = tests
TARGET = mandelbrot
TEST_TARGET = $(BUILD_DIR)/test_runner
SRC = main.cpp mandelbrot_renderer.cpp mandelbrot_calculator.cpp info_window.cpp
TEST_SRC = $(TEST_DIR)/test_mandelbrot_calculator.cpp $(TEST_DIR)/test_mandelbrot_renderer.cpp
OBJ = $(BUILD_DIR)/main.o $(BUILD_DIR)/mandelbrot_renderer.o $(BUILD_DIR)/mandelbrot_calculator.o $(BUILD_DIR)/info_window.o
TEST_OBJ = $(BUILD_DIR)/mandelbrot_calculator.o $(BUILD_DIR)/mandelbrot_renderer_test.o $(BUILD_DIR)/info_window.o \
           $(BUILD_DIR)/test_mandelbrot_calculator.o $(BUILD_DIR)/test_mandelbrot_renderer.o
LIBS = -lglfw -lGL -lpthread -lfreetype

PREFIX ?= /usr/local
BINDIR = $(PREFIX)/bin
MANDIR = $(PREFIX)/share/man/man1

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
	$(MAKE) deb-clean
	rm -f $(TARGET)
	rm -rf $(BUILD_DIR)

install: $(TARGET)
	install -D -m 0755 $(TARGET) $(DESTDIR)$(BINDIR)/$(TARGET)
	install -D -m 0644 man/mandelbrot.1 $(DESTDIR)$(MANDIR)/mandelbrot.1

uninstall:
	rm -f $(DESTDIR)$(BINDIR)/$(TARGET)
	rm -f $(DESTDIR)$(MANDIR)/mandelbrot.1

run: $(TARGET)
	./$(TARGET)

# Test targets
test: $(TEST_TARGET)
	@echo "Running tests..."
	@$(TEST_TARGET)

$(TEST_TARGET): $(TEST_OBJ)
	$(CXX) $(CXXFLAGS) -o $(TEST_TARGET) $(TEST_OBJ) $(LIBS)

$(BUILD_DIR)/mandelbrot_renderer_test.o: mandelbrot_renderer.cpp mandelbrot_renderer.h | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -DTESTING -c mandelbrot_renderer.cpp -o $(BUILD_DIR)/mandelbrot_renderer_test.o

$(BUILD_DIR)/test_mandelbrot_calculator.o: $(TEST_DIR)/test_mandelbrot_calculator.cpp mandelbrot_calculator.h $(TEST_DIR)/catch.hpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -I$(TEST_DIR) -c $(TEST_DIR)/test_mandelbrot_calculator.cpp -o $(BUILD_DIR)/test_mandelbrot_calculator.o

$(BUILD_DIR)/test_mandelbrot_renderer.o: $(TEST_DIR)/test_mandelbrot_renderer.cpp mandelbrot_renderer.h $(TEST_DIR)/catch.hpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -I$(TEST_DIR) -DTESTING -c $(TEST_DIR)/test_mandelbrot_renderer.cpp -o $(BUILD_DIR)/test_mandelbrot_renderer.o

deb: clean
	dpkg-buildpackage -us -uc -b

deb-clean:
	-fakeroot debian/rules clean

.PHONY: all clean install uninstall run test deb deb-clean
