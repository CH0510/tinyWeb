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
CMAKE_SOURCE_DIR = /home/chenhao/tinyWeb/base

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/chenhao/tinyWeb/base/build

# Include any dependencies generated for this target.
include CMakeFiles/Thread_bench.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/Thread_bench.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Thread_bench.dir/flags.make

CMakeFiles/Thread_bench.dir/Thread_bench.cc.o: CMakeFiles/Thread_bench.dir/flags.make
CMakeFiles/Thread_bench.dir/Thread_bench.cc.o: ../Thread_bench.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/chenhao/tinyWeb/base/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/Thread_bench.dir/Thread_bench.cc.o"
	g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Thread_bench.dir/Thread_bench.cc.o -c /home/chenhao/tinyWeb/base/Thread_bench.cc

CMakeFiles/Thread_bench.dir/Thread_bench.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Thread_bench.dir/Thread_bench.cc.i"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/chenhao/tinyWeb/base/Thread_bench.cc > CMakeFiles/Thread_bench.dir/Thread_bench.cc.i

CMakeFiles/Thread_bench.dir/Thread_bench.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Thread_bench.dir/Thread_bench.cc.s"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/chenhao/tinyWeb/base/Thread_bench.cc -o CMakeFiles/Thread_bench.dir/Thread_bench.cc.s

# Object files for target Thread_bench
Thread_bench_OBJECTS = \
"CMakeFiles/Thread_bench.dir/Thread_bench.cc.o"

# External object files for target Thread_bench
Thread_bench_EXTERNAL_OBJECTS =

bin/Thread_bench: CMakeFiles/Thread_bench.dir/Thread_bench.cc.o
bin/Thread_bench: CMakeFiles/Thread_bench.dir/build.make
bin/Thread_bench: ../lib/libtinyWeb_base.a
bin/Thread_bench: CMakeFiles/Thread_bench.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/chenhao/tinyWeb/base/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable bin/Thread_bench"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Thread_bench.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Thread_bench.dir/build: bin/Thread_bench

.PHONY : CMakeFiles/Thread_bench.dir/build

CMakeFiles/Thread_bench.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/Thread_bench.dir/cmake_clean.cmake
.PHONY : CMakeFiles/Thread_bench.dir/clean

CMakeFiles/Thread_bench.dir/depend:
	cd /home/chenhao/tinyWeb/base/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/chenhao/tinyWeb/base /home/chenhao/tinyWeb/base /home/chenhao/tinyWeb/base/build /home/chenhao/tinyWeb/base/build /home/chenhao/tinyWeb/base/build/CMakeFiles/Thread_bench.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/Thread_bench.dir/depend

