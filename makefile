# Useful: https://makefiletutorial.com/

# the compilers: CC = c-files, CXX = c++ files
CC := nvcc
CXX := nvcc
# compiler flags:
#	-g	- this flag adds debugging information to the executable file
CPPFLAGS := -g 

EXT_LIB_DIRS := -L${MILK_INSTALLDIR}/lib/
#	-lcurses			- ncurses library for CLI
#	-lm3api				- xiAPI for interfacing with the camera
#	-lImageStreamIO		- Interfacing with milk data streams
LDFLAGS := $(EXT_LIB_DIRS) -lncurses -lm3api -lImageStreamIO
EXT_INC_DIRS := -I${MILK_INSTALLDIR}/include/ImageStreamIO/

AUTORUN_AFTER_BUILD := false

# =========================

# Thanks to Job Vranish (https://spin.atomicobject.com/2016/08/26/makefile-c-projects/)
TARGET_EXEC := xiB-64-2-milk

BUILD_DIR := ./build
SRC_DIRS := ./src

# Find all the C and C++ files we want to compile
# Note the single quotes around the * expressions. Make will incorrectly expand these otherwise.
SRCS := $(shell find $(SRC_DIRS) -name '*.cpp' -or -name '*.c' -or -name '*.s')

# String substitution for every C/C++ file.
# As an example, hello.cpp turns into ./build/hello.cpp.o
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)

# String substitution (suffix version without %).
# As an example, ./build/hello.cpp.o turns into ./build/hello.cpp.d
DEPS := $(OBJS:.o=.d)

# Every folder in ./src will need to be passed to GCC so that it can find header files
INC_DIRS := $(shell find $(SRC_DIRS) -type d)
# Add a prefix to INC_DIRS. So moduleA would become -ImoduleA. GCC understands this -I flag
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

# The -MMD and -MP flags together generate Makefiles for us!
# These files will have .d instead of .o as the output.
CPPFLAGS := $(CPPFLAGS) $(INC_FLAGS) $(EXT_INC_DIRS) -MMD -MP

# The final build step.
# Extended by an automatic run if $(AUTORUN_AFTER_BUILD) equals true
$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)
	@if [ ! -f build/libImageStreamIO.so ]; then ln -s ${MILK_INSTALLDIR}/lib/libImageStreamIO.so build/libImageStreamIO.so; fi
ifeq ($(AUTORUN_AFTER_BUILD), true)
	@echo "\n\n=== Execute application ===\n"
	@make run | grep -v make
else
	@echo "\n\nBuild finished. Autorun deactivated."
endif
	

# Build step for C source
$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# Build step for C++ source
$(BUILD_DIR)/%.cpp.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@


.PHONY: clean
clean:
	rm -r $(BUILD_DIR)

# Custom: Shortcut to run the application
run: build/$(TARGET_EXEC)
	@(cd build; ./$(TARGET_EXEC))

# Include the .d makefiles. The - at the front suppresses the errors of missing
# Makefiles. Initially, all the .d files will be missing, and we don't want those
# errors to show up.
-include $(DEPS)
