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
include src/CMakeFiles/tinyWeb.dir/depend.make

# Include the progress variables for this target.
include src/CMakeFiles/tinyWeb.dir/progress.make

# Include the compile flags for this target's objects.
include src/CMakeFiles/tinyWeb.dir/flags.make

src/CMakeFiles/tinyWeb.dir/AsyncLogging.cc.o: src/CMakeFiles/tinyWeb.dir/flags.make
src/CMakeFiles/tinyWeb.dir/AsyncLogging.cc.o: ../src/AsyncLogging.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/chenhao/tinyWeb/base/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/CMakeFiles/tinyWeb.dir/AsyncLogging.cc.o"
	cd /home/chenhao/tinyWeb/base/build/src && g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/tinyWeb.dir/AsyncLogging.cc.o -c /home/chenhao/tinyWeb/base/src/AsyncLogging.cc

src/CMakeFiles/tinyWeb.dir/AsyncLogging.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/tinyWeb.dir/AsyncLogging.cc.i"
	cd /home/chenhao/tinyWeb/base/build/src && g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/chenhao/tinyWeb/base/src/AsyncLogging.cc > CMakeFiles/tinyWeb.dir/AsyncLogging.cc.i

src/CMakeFiles/tinyWeb.dir/AsyncLogging.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/tinyWeb.dir/AsyncLogging.cc.s"
	cd /home/chenhao/tinyWeb/base/build/src && g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/chenhao/tinyWeb/base/src/AsyncLogging.cc -o CMakeFiles/tinyWeb.dir/AsyncLogging.cc.s

src/CMakeFiles/tinyWeb.dir/CountDownLatch.cc.o: src/CMakeFiles/tinyWeb.dir/flags.make
src/CMakeFiles/tinyWeb.dir/CountDownLatch.cc.o: ../src/CountDownLatch.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/chenhao/tinyWeb/base/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object src/CMakeFiles/tinyWeb.dir/CountDownLatch.cc.o"
	cd /home/chenhao/tinyWeb/base/build/src && g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/tinyWeb.dir/CountDownLatch.cc.o -c /home/chenhao/tinyWeb/base/src/CountDownLatch.cc

src/CMakeFiles/tinyWeb.dir/CountDownLatch.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/tinyWeb.dir/CountDownLatch.cc.i"
	cd /home/chenhao/tinyWeb/base/build/src && g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/chenhao/tinyWeb/base/src/CountDownLatch.cc > CMakeFiles/tinyWeb.dir/CountDownLatch.cc.i

src/CMakeFiles/tinyWeb.dir/CountDownLatch.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/tinyWeb.dir/CountDownLatch.cc.s"
	cd /home/chenhao/tinyWeb/base/build/src && g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/chenhao/tinyWeb/base/src/CountDownLatch.cc -o CMakeFiles/tinyWeb.dir/CountDownLatch.cc.s

src/CMakeFiles/tinyWeb.dir/CurrentThread.cc.o: src/CMakeFiles/tinyWeb.dir/flags.make
src/CMakeFiles/tinyWeb.dir/CurrentThread.cc.o: ../src/CurrentThread.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/chenhao/tinyWeb/base/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object src/CMakeFiles/tinyWeb.dir/CurrentThread.cc.o"
	cd /home/chenhao/tinyWeb/base/build/src && g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/tinyWeb.dir/CurrentThread.cc.o -c /home/chenhao/tinyWeb/base/src/CurrentThread.cc

src/CMakeFiles/tinyWeb.dir/CurrentThread.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/tinyWeb.dir/CurrentThread.cc.i"
	cd /home/chenhao/tinyWeb/base/build/src && g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/chenhao/tinyWeb/base/src/CurrentThread.cc > CMakeFiles/tinyWeb.dir/CurrentThread.cc.i

src/CMakeFiles/tinyWeb.dir/CurrentThread.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/tinyWeb.dir/CurrentThread.cc.s"
	cd /home/chenhao/tinyWeb/base/build/src && g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/chenhao/tinyWeb/base/src/CurrentThread.cc -o CMakeFiles/tinyWeb.dir/CurrentThread.cc.s

src/CMakeFiles/tinyWeb.dir/FileUtil.cc.o: src/CMakeFiles/tinyWeb.dir/flags.make
src/CMakeFiles/tinyWeb.dir/FileUtil.cc.o: ../src/FileUtil.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/chenhao/tinyWeb/base/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object src/CMakeFiles/tinyWeb.dir/FileUtil.cc.o"
	cd /home/chenhao/tinyWeb/base/build/src && g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/tinyWeb.dir/FileUtil.cc.o -c /home/chenhao/tinyWeb/base/src/FileUtil.cc

src/CMakeFiles/tinyWeb.dir/FileUtil.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/tinyWeb.dir/FileUtil.cc.i"
	cd /home/chenhao/tinyWeb/base/build/src && g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/chenhao/tinyWeb/base/src/FileUtil.cc > CMakeFiles/tinyWeb.dir/FileUtil.cc.i

src/CMakeFiles/tinyWeb.dir/FileUtil.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/tinyWeb.dir/FileUtil.cc.s"
	cd /home/chenhao/tinyWeb/base/build/src && g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/chenhao/tinyWeb/base/src/FileUtil.cc -o CMakeFiles/tinyWeb.dir/FileUtil.cc.s

src/CMakeFiles/tinyWeb.dir/LogFile.cc.o: src/CMakeFiles/tinyWeb.dir/flags.make
src/CMakeFiles/tinyWeb.dir/LogFile.cc.o: ../src/LogFile.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/chenhao/tinyWeb/base/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object src/CMakeFiles/tinyWeb.dir/LogFile.cc.o"
	cd /home/chenhao/tinyWeb/base/build/src && g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/tinyWeb.dir/LogFile.cc.o -c /home/chenhao/tinyWeb/base/src/LogFile.cc

src/CMakeFiles/tinyWeb.dir/LogFile.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/tinyWeb.dir/LogFile.cc.i"
	cd /home/chenhao/tinyWeb/base/build/src && g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/chenhao/tinyWeb/base/src/LogFile.cc > CMakeFiles/tinyWeb.dir/LogFile.cc.i

src/CMakeFiles/tinyWeb.dir/LogFile.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/tinyWeb.dir/LogFile.cc.s"
	cd /home/chenhao/tinyWeb/base/build/src && g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/chenhao/tinyWeb/base/src/LogFile.cc -o CMakeFiles/tinyWeb.dir/LogFile.cc.s

src/CMakeFiles/tinyWeb.dir/LogStream.cc.o: src/CMakeFiles/tinyWeb.dir/flags.make
src/CMakeFiles/tinyWeb.dir/LogStream.cc.o: ../src/LogStream.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/chenhao/tinyWeb/base/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object src/CMakeFiles/tinyWeb.dir/LogStream.cc.o"
	cd /home/chenhao/tinyWeb/base/build/src && g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/tinyWeb.dir/LogStream.cc.o -c /home/chenhao/tinyWeb/base/src/LogStream.cc

src/CMakeFiles/tinyWeb.dir/LogStream.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/tinyWeb.dir/LogStream.cc.i"
	cd /home/chenhao/tinyWeb/base/build/src && g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/chenhao/tinyWeb/base/src/LogStream.cc > CMakeFiles/tinyWeb.dir/LogStream.cc.i

src/CMakeFiles/tinyWeb.dir/LogStream.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/tinyWeb.dir/LogStream.cc.s"
	cd /home/chenhao/tinyWeb/base/build/src && g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/chenhao/tinyWeb/base/src/LogStream.cc -o CMakeFiles/tinyWeb.dir/LogStream.cc.s

src/CMakeFiles/tinyWeb.dir/Logging.cc.o: src/CMakeFiles/tinyWeb.dir/flags.make
src/CMakeFiles/tinyWeb.dir/Logging.cc.o: ../src/Logging.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/chenhao/tinyWeb/base/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object src/CMakeFiles/tinyWeb.dir/Logging.cc.o"
	cd /home/chenhao/tinyWeb/base/build/src && g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/tinyWeb.dir/Logging.cc.o -c /home/chenhao/tinyWeb/base/src/Logging.cc

src/CMakeFiles/tinyWeb.dir/Logging.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/tinyWeb.dir/Logging.cc.i"
	cd /home/chenhao/tinyWeb/base/build/src && g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/chenhao/tinyWeb/base/src/Logging.cc > CMakeFiles/tinyWeb.dir/Logging.cc.i

src/CMakeFiles/tinyWeb.dir/Logging.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/tinyWeb.dir/Logging.cc.s"
	cd /home/chenhao/tinyWeb/base/build/src && g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/chenhao/tinyWeb/base/src/Logging.cc -o CMakeFiles/tinyWeb.dir/Logging.cc.s

src/CMakeFiles/tinyWeb.dir/Thread.cc.o: src/CMakeFiles/tinyWeb.dir/flags.make
src/CMakeFiles/tinyWeb.dir/Thread.cc.o: ../src/Thread.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/chenhao/tinyWeb/base/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building CXX object src/CMakeFiles/tinyWeb.dir/Thread.cc.o"
	cd /home/chenhao/tinyWeb/base/build/src && g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/tinyWeb.dir/Thread.cc.o -c /home/chenhao/tinyWeb/base/src/Thread.cc

src/CMakeFiles/tinyWeb.dir/Thread.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/tinyWeb.dir/Thread.cc.i"
	cd /home/chenhao/tinyWeb/base/build/src && g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/chenhao/tinyWeb/base/src/Thread.cc > CMakeFiles/tinyWeb.dir/Thread.cc.i

src/CMakeFiles/tinyWeb.dir/Thread.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/tinyWeb.dir/Thread.cc.s"
	cd /home/chenhao/tinyWeb/base/build/src && g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/chenhao/tinyWeb/base/src/Thread.cc -o CMakeFiles/tinyWeb.dir/Thread.cc.s

src/CMakeFiles/tinyWeb.dir/ThreadPool.cc.o: src/CMakeFiles/tinyWeb.dir/flags.make
src/CMakeFiles/tinyWeb.dir/ThreadPool.cc.o: ../src/ThreadPool.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/chenhao/tinyWeb/base/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building CXX object src/CMakeFiles/tinyWeb.dir/ThreadPool.cc.o"
	cd /home/chenhao/tinyWeb/base/build/src && g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/tinyWeb.dir/ThreadPool.cc.o -c /home/chenhao/tinyWeb/base/src/ThreadPool.cc

src/CMakeFiles/tinyWeb.dir/ThreadPool.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/tinyWeb.dir/ThreadPool.cc.i"
	cd /home/chenhao/tinyWeb/base/build/src && g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/chenhao/tinyWeb/base/src/ThreadPool.cc > CMakeFiles/tinyWeb.dir/ThreadPool.cc.i

src/CMakeFiles/tinyWeb.dir/ThreadPool.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/tinyWeb.dir/ThreadPool.cc.s"
	cd /home/chenhao/tinyWeb/base/build/src && g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/chenhao/tinyWeb/base/src/ThreadPool.cc -o CMakeFiles/tinyWeb.dir/ThreadPool.cc.s

src/CMakeFiles/tinyWeb.dir/Timestamp.cc.o: src/CMakeFiles/tinyWeb.dir/flags.make
src/CMakeFiles/tinyWeb.dir/Timestamp.cc.o: ../src/Timestamp.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/chenhao/tinyWeb/base/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Building CXX object src/CMakeFiles/tinyWeb.dir/Timestamp.cc.o"
	cd /home/chenhao/tinyWeb/base/build/src && g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/tinyWeb.dir/Timestamp.cc.o -c /home/chenhao/tinyWeb/base/src/Timestamp.cc

src/CMakeFiles/tinyWeb.dir/Timestamp.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/tinyWeb.dir/Timestamp.cc.i"
	cd /home/chenhao/tinyWeb/base/build/src && g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/chenhao/tinyWeb/base/src/Timestamp.cc > CMakeFiles/tinyWeb.dir/Timestamp.cc.i

src/CMakeFiles/tinyWeb.dir/Timestamp.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/tinyWeb.dir/Timestamp.cc.s"
	cd /home/chenhao/tinyWeb/base/build/src && g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/chenhao/tinyWeb/base/src/Timestamp.cc -o CMakeFiles/tinyWeb.dir/Timestamp.cc.s

# Object files for target tinyWeb
tinyWeb_OBJECTS = \
"CMakeFiles/tinyWeb.dir/AsyncLogging.cc.o" \
"CMakeFiles/tinyWeb.dir/CountDownLatch.cc.o" \
"CMakeFiles/tinyWeb.dir/CurrentThread.cc.o" \
"CMakeFiles/tinyWeb.dir/FileUtil.cc.o" \
"CMakeFiles/tinyWeb.dir/LogFile.cc.o" \
"CMakeFiles/tinyWeb.dir/LogStream.cc.o" \
"CMakeFiles/tinyWeb.dir/Logging.cc.o" \
"CMakeFiles/tinyWeb.dir/Thread.cc.o" \
"CMakeFiles/tinyWeb.dir/ThreadPool.cc.o" \
"CMakeFiles/tinyWeb.dir/Timestamp.cc.o"

# External object files for target tinyWeb
tinyWeb_EXTERNAL_OBJECTS =

../lib/libtinyWeb.a: src/CMakeFiles/tinyWeb.dir/AsyncLogging.cc.o
../lib/libtinyWeb.a: src/CMakeFiles/tinyWeb.dir/CountDownLatch.cc.o
../lib/libtinyWeb.a: src/CMakeFiles/tinyWeb.dir/CurrentThread.cc.o
../lib/libtinyWeb.a: src/CMakeFiles/tinyWeb.dir/FileUtil.cc.o
../lib/libtinyWeb.a: src/CMakeFiles/tinyWeb.dir/LogFile.cc.o
../lib/libtinyWeb.a: src/CMakeFiles/tinyWeb.dir/LogStream.cc.o
../lib/libtinyWeb.a: src/CMakeFiles/tinyWeb.dir/Logging.cc.o
../lib/libtinyWeb.a: src/CMakeFiles/tinyWeb.dir/Thread.cc.o
../lib/libtinyWeb.a: src/CMakeFiles/tinyWeb.dir/ThreadPool.cc.o
../lib/libtinyWeb.a: src/CMakeFiles/tinyWeb.dir/Timestamp.cc.o
../lib/libtinyWeb.a: src/CMakeFiles/tinyWeb.dir/build.make
../lib/libtinyWeb.a: src/CMakeFiles/tinyWeb.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/chenhao/tinyWeb/base/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_11) "Linking CXX static library ../../lib/libtinyWeb.a"
	cd /home/chenhao/tinyWeb/base/build/src && $(CMAKE_COMMAND) -P CMakeFiles/tinyWeb.dir/cmake_clean_target.cmake
	cd /home/chenhao/tinyWeb/base/build/src && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/tinyWeb.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/CMakeFiles/tinyWeb.dir/build: ../lib/libtinyWeb.a

.PHONY : src/CMakeFiles/tinyWeb.dir/build

src/CMakeFiles/tinyWeb.dir/clean:
	cd /home/chenhao/tinyWeb/base/build/src && $(CMAKE_COMMAND) -P CMakeFiles/tinyWeb.dir/cmake_clean.cmake
.PHONY : src/CMakeFiles/tinyWeb.dir/clean

src/CMakeFiles/tinyWeb.dir/depend:
	cd /home/chenhao/tinyWeb/base/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/chenhao/tinyWeb/base /home/chenhao/tinyWeb/base/src /home/chenhao/tinyWeb/base/build /home/chenhao/tinyWeb/base/build/src /home/chenhao/tinyWeb/base/build/src/CMakeFiles/tinyWeb.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/CMakeFiles/tinyWeb.dir/depend

