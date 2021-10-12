# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/chenhao/tinyWeb/net

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/chenhao/tinyWeb/net/build

# Include any dependencies generated for this target.
include src/CMakeFiles/tinyWebNet.dir/depend.make

# Include the progress variables for this target.
include src/CMakeFiles/tinyWebNet.dir/progress.make

# Include the compile flags for this target's objects.
include src/CMakeFiles/tinyWebNet.dir/flags.make

src/CMakeFiles/tinyWebNet.dir/EventLoop.cc.o: src/CMakeFiles/tinyWebNet.dir/flags.make
src/CMakeFiles/tinyWebNet.dir/EventLoop.cc.o: ../src/EventLoop.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/chenhao/tinyWeb/net/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/CMakeFiles/tinyWebNet.dir/EventLoop.cc.o"
	cd /home/chenhao/tinyWeb/net/build/src && g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/tinyWebNet.dir/EventLoop.cc.o -c /home/chenhao/tinyWeb/net/src/EventLoop.cc

src/CMakeFiles/tinyWebNet.dir/EventLoop.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/tinyWebNet.dir/EventLoop.cc.i"
	cd /home/chenhao/tinyWeb/net/build/src && g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/chenhao/tinyWeb/net/src/EventLoop.cc > CMakeFiles/tinyWebNet.dir/EventLoop.cc.i

src/CMakeFiles/tinyWebNet.dir/EventLoop.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/tinyWebNet.dir/EventLoop.cc.s"
	cd /home/chenhao/tinyWeb/net/build/src && g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/chenhao/tinyWeb/net/src/EventLoop.cc -o CMakeFiles/tinyWebNet.dir/EventLoop.cc.s

# Object files for target tinyWebNet
tinyWebNet_OBJECTS = \
"CMakeFiles/tinyWebNet.dir/EventLoop.cc.o"

# External object files for target tinyWebNet
tinyWebNet_EXTERNAL_OBJECTS =

../lib/libtinyWebNet.a: src/CMakeFiles/tinyWebNet.dir/EventLoop.cc.o
../lib/libtinyWebNet.a: src/CMakeFiles/tinyWebNet.dir/build.make
../lib/libtinyWebNet.a: src/CMakeFiles/tinyWebNet.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/chenhao/tinyWeb/net/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library ../../lib/libtinyWebNet.a"
	cd /home/chenhao/tinyWeb/net/build/src && $(CMAKE_COMMAND) -P CMakeFiles/tinyWebNet.dir/cmake_clean_target.cmake
	cd /home/chenhao/tinyWeb/net/build/src && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/tinyWebNet.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/CMakeFiles/tinyWebNet.dir/build: ../lib/libtinyWebNet.a

.PHONY : src/CMakeFiles/tinyWebNet.dir/build

src/CMakeFiles/tinyWebNet.dir/clean:
	cd /home/chenhao/tinyWeb/net/build/src && $(CMAKE_COMMAND) -P CMakeFiles/tinyWebNet.dir/cmake_clean.cmake
.PHONY : src/CMakeFiles/tinyWebNet.dir/clean

src/CMakeFiles/tinyWebNet.dir/depend:
	cd /home/chenhao/tinyWeb/net/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/chenhao/tinyWeb/net /home/chenhao/tinyWeb/net/src /home/chenhao/tinyWeb/net/build /home/chenhao/tinyWeb/net/build/src /home/chenhao/tinyWeb/net/build/src/CMakeFiles/tinyWebNet.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/CMakeFiles/tinyWebNet.dir/depend

