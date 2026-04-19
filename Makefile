BUILD_DIR = build
EXECUTABLE = start
CMAKE_GENERATOR = "Unix Makefiles"
BUILD_TYPE = Release

.PHONY: all clean rebuild run

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
	@./$(BUILD_DIR)/$(EXECUTABLE)
