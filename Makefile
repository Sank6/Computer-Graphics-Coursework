PROJECT_NAME := ComputerGraphics
BUILD_DIR := build

# Define the names of key files
SOURCE_FILES := $(shell find src -name '*.cpp')
OBJECT_FILES := $(patsubst src/%.cpp, $(BUILD_DIR)/%.o, $(SOURCE_FILES))
EXECUTABLE := $(BUILD_DIR)/$(PROJECT_NAME)
SDW_DIR := ./libs/sdw/
GLM_DIR := ./libs/glm-0.9.7.2/
SDW_SOURCE_FILES := $(wildcard $(SDW_DIR)*.cpp)
SDW_OBJECT_FILES := $(patsubst $(SDW_DIR)%.cpp, $(BUILD_DIR)/%.o, $(SDW_SOURCE_FILES))

# Build settings
COMPILER := clang++
COMPILER_OPTIONS := -c -pipe -Wall -std=c++11
DEBUG_OPTIONS := -ggdb -g3
FUSSY_OPTIONS := -Werror -pedantic
SANITIZER_OPTIONS := -O1 -fsanitize=undefined -fsanitize=address -fno-omit-frame-pointer
SPEEDY_OPTIONS := -Ofast -funsafe-math-optimizations -march=native
LINKER_OPTIONS :=

# Set up flags
SDW_COMPILER_FLAGS := -I$(SDW_DIR)
GLM_COMPILER_FLAGS := -I$(GLM_DIR)
SDL_COMPILER_FLAGS := $(shell sdl2-config --cflags)
SDL_LINKER_FLAGS := $(shell sdl2-config --libs)
SDW_LINKER_FLAGS := $(SDW_OBJECT_FILES)

.PHONY: default debug diagnostic speedy production clean

default: debug

debug: $(EXECUTABLE)
	./$(EXECUTABLE)

diagnostic: $(EXECUTABLE)
	./$(EXECUTABLE)

speedy: $(EXECUTABLE)
	./$(EXECUTABLE)

production: $(EXECUTABLE)
	./$(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECT_FILES) $(SDW_OBJECT_FILES)
	mkdir -p $(dir $@)
	$(COMPILER) $(LINKER_OPTIONS) $(SPEEDY_OPTIONS) -o $@ $(OBJECT_FILES) $(SDW_OBJECT_FILES) $(SDL_LINKER_FLAGS)

$(BUILD_DIR)/%.o: $(SDW_DIR)%.cpp
	mkdir -p $(dir $@)
	$(COMPILER) $(COMPILER_OPTIONS) $(SPEEDY_OPTIONS) -c $< -o $@ $(SDL_COMPILER_FLAGS) $(GLM_COMPILER_FLAGS)

$(BUILD_DIR)/%.o: src/%.cpp
	mkdir -p $(dir $@)
	$(COMPILER) $(COMPILER_OPTIONS) $(SPEEDY_OPTIONS) -c $< -o $@ $(SDL_COMPILER_FLAGS) $(SDW_COMPILER_FLAGS) $(GLM_COMPILER_FLAGS)

clean:
	rm -rf $(BUILD_DIR)/*.o $(BUILD_DIR)/$(PROJECT_NAME)
