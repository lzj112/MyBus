# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

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
CMAKE_SOURCE_DIR = /home/lzj/MyBus/mybus/demo

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/lzj/MyBus/mybus/demo/build

# Include any dependencies generated for this target.
include CMakeFiles/transit2.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/transit2.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/transit2.dir/flags.make

CMakeFiles/transit2.dir/BusServer2.cpp.o: CMakeFiles/transit2.dir/flags.make
CMakeFiles/transit2.dir/BusServer2.cpp.o: ../BusServer2.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/lzj/MyBus/mybus/demo/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/transit2.dir/BusServer2.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/transit2.dir/BusServer2.cpp.o -c /home/lzj/MyBus/mybus/demo/BusServer2.cpp

CMakeFiles/transit2.dir/BusServer2.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/transit2.dir/BusServer2.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/lzj/MyBus/mybus/demo/BusServer2.cpp > CMakeFiles/transit2.dir/BusServer2.cpp.i

CMakeFiles/transit2.dir/BusServer2.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/transit2.dir/BusServer2.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/lzj/MyBus/mybus/demo/BusServer2.cpp -o CMakeFiles/transit2.dir/BusServer2.cpp.s

CMakeFiles/transit2.dir/BusServer2.cpp.o.requires:

.PHONY : CMakeFiles/transit2.dir/BusServer2.cpp.o.requires

CMakeFiles/transit2.dir/BusServer2.cpp.o.provides: CMakeFiles/transit2.dir/BusServer2.cpp.o.requires
	$(MAKE) -f CMakeFiles/transit2.dir/build.make CMakeFiles/transit2.dir/BusServer2.cpp.o.provides.build
.PHONY : CMakeFiles/transit2.dir/BusServer2.cpp.o.provides

CMakeFiles/transit2.dir/BusServer2.cpp.o.provides.build: CMakeFiles/transit2.dir/BusServer2.cpp.o


# Object files for target transit2
transit2_OBJECTS = \
"CMakeFiles/transit2.dir/BusServer2.cpp.o"

# External object files for target transit2
transit2_EXTERNAL_OBJECTS =

transit2: CMakeFiles/transit2.dir/BusServer2.cpp.o
transit2: CMakeFiles/transit2.dir/build.make
transit2: src/libMyBus.a
transit2: CMakeFiles/transit2.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/lzj/MyBus/mybus/demo/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable transit2"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/transit2.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/transit2.dir/build: transit2

.PHONY : CMakeFiles/transit2.dir/build

CMakeFiles/transit2.dir/requires: CMakeFiles/transit2.dir/BusServer2.cpp.o.requires

.PHONY : CMakeFiles/transit2.dir/requires

CMakeFiles/transit2.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/transit2.dir/cmake_clean.cmake
.PHONY : CMakeFiles/transit2.dir/clean

CMakeFiles/transit2.dir/depend:
	cd /home/lzj/MyBus/mybus/demo/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/lzj/MyBus/mybus/demo /home/lzj/MyBus/mybus/demo /home/lzj/MyBus/mybus/demo/build /home/lzj/MyBus/mybus/demo/build /home/lzj/MyBus/mybus/demo/build/CMakeFiles/transit2.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/transit2.dir/depend

