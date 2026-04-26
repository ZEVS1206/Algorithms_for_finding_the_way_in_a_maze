BUILD_DIR = build
EXECUTABLE = start
CMAKE_GENERATOR = "Unix Makefiles"
BUILD_TYPE = Release
SOURCE = source

file ?=

.PHONY: all clean rebuild run generate visualize_one visualize_all get_statistics test_all

all: $(BUILD_DIR)/Makefile
	@$(MAKE) -C $(BUILD_DIR) --no-print-directory

$(BUILD_DIR)/Makefile:
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && cmake .. -G $(CMAKE_GENERATOR) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)
	@echo "CMake configuration done."

clean:
	@rm -rf $(BUILD_DIR)
	@echo "Build directory removed."

rebuild: clean all

run: all
	@./$(BUILD_DIR)/$(EXECUTABLE) $(file)
generate:
	python3 $(SOURCE)/generate_mazes.py
visualize_one:
	python3 $(SOURCE)/visualize_one_maze.py
visualize_all:
	python3 $(SOURCE)/visualize_all.py
get_statistics:
	python3 $(SOURCE)/get_statistics.py
test_all:
	python3 $(SOURCE)/main_run.py
