# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

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
CMAKE_SOURCE_DIR = /home/supreme/djk/myServer

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/supreme/djk/myServer/build

# Include any dependencies generated for this target.
include myServer/client/CMakeFiles/client.dir/depend.make

# Include the progress variables for this target.
include myServer/client/CMakeFiles/client.dir/progress.make

# Include the compile flags for this target's objects.
include myServer/client/CMakeFiles/client.dir/flags.make

myServer/client/CMakeFiles/client.dir/client.cc.o: myServer/client/CMakeFiles/client.dir/flags.make
myServer/client/CMakeFiles/client.dir/client.cc.o: ../myServer/client/client.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/supreme/djk/myServer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object myServer/client/CMakeFiles/client.dir/client.cc.o"
	cd /home/supreme/djk/myServer/build/myServer/client && /usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/client.dir/client.cc.o -c /home/supreme/djk/myServer/myServer/client/client.cc

myServer/client/CMakeFiles/client.dir/client.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/client.dir/client.cc.i"
	cd /home/supreme/djk/myServer/build/myServer/client && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/supreme/djk/myServer/myServer/client/client.cc > CMakeFiles/client.dir/client.cc.i

myServer/client/CMakeFiles/client.dir/client.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/client.dir/client.cc.s"
	cd /home/supreme/djk/myServer/build/myServer/client && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/supreme/djk/myServer/myServer/client/client.cc -o CMakeFiles/client.dir/client.cc.s

myServer/client/CMakeFiles/client.dir/client.cc.o.requires:

.PHONY : myServer/client/CMakeFiles/client.dir/client.cc.o.requires

myServer/client/CMakeFiles/client.dir/client.cc.o.provides: myServer/client/CMakeFiles/client.dir/client.cc.o.requires
	$(MAKE) -f myServer/client/CMakeFiles/client.dir/build.make myServer/client/CMakeFiles/client.dir/client.cc.o.provides.build
.PHONY : myServer/client/CMakeFiles/client.dir/client.cc.o.provides

myServer/client/CMakeFiles/client.dir/client.cc.o.provides.build: myServer/client/CMakeFiles/client.dir/client.cc.o


# Object files for target client
client_OBJECTS = \
"CMakeFiles/client.dir/client.cc.o"

# External object files for target client
client_EXTERNAL_OBJECTS =

myServer/client/client: myServer/client/CMakeFiles/client.dir/client.cc.o
myServer/client/client: myServer/client/CMakeFiles/client.dir/build.make
myServer/client/client: myServer/client/CMakeFiles/client.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/supreme/djk/myServer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable client"
	cd /home/supreme/djk/myServer/build/myServer/client && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/client.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
myServer/client/CMakeFiles/client.dir/build: myServer/client/client

.PHONY : myServer/client/CMakeFiles/client.dir/build

myServer/client/CMakeFiles/client.dir/requires: myServer/client/CMakeFiles/client.dir/client.cc.o.requires

.PHONY : myServer/client/CMakeFiles/client.dir/requires

myServer/client/CMakeFiles/client.dir/clean:
	cd /home/supreme/djk/myServer/build/myServer/client && $(CMAKE_COMMAND) -P CMakeFiles/client.dir/cmake_clean.cmake
.PHONY : myServer/client/CMakeFiles/client.dir/clean

myServer/client/CMakeFiles/client.dir/depend:
	cd /home/supreme/djk/myServer/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/supreme/djk/myServer /home/supreme/djk/myServer/myServer/client /home/supreme/djk/myServer/build /home/supreme/djk/myServer/build/myServer/client /home/supreme/djk/myServer/build/myServer/client/CMakeFiles/client.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : myServer/client/CMakeFiles/client.dir/depend

