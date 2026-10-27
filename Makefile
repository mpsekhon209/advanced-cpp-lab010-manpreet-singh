CXX := g++
CXXFLAGS := -std=c++17 -Wall -Itests -Isrc -Iinclude

SRC_DIR := src
TEST_DIR := tests
BUILD_DIR := build

# Source files for the hash table library
SRC := $(wildcard $(SRC_DIR)/*.cpp)
OBJ := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRC))

# Test sources (these contain Catch2 main)
TEST_SRC := $(wildcard $(TEST_DIR)/*.cpp)
TEST_OBJ := $(patsubst $(TEST_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(TEST_SRC))
TEST_EXECUTABLE := $(BUILD_DIR)/test.out

.PHONY: all clean test

all: test

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile test .cpp files
$(BUILD_DIR)/%.o: $(TEST_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Build and run tests — IMPORTANT: DO NOT LINK main.o
test: $(OBJ) $(TEST_OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) $(TEST_OBJ) -o $(TEST_EXECUTABLE)
	./$(TEST_EXECUTABLE)

clean:
	rm -rf $(BUILD_DIR)

