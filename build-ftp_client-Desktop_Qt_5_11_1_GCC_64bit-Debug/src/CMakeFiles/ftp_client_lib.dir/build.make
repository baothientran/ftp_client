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
CMAKE_SOURCE_DIR = /home/baothientran/Projects/ftp_client

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/baothientran/Projects/ftp_client/build-ftp_client-Desktop_Qt_5_11_1_GCC_64bit-Debug

# Include any dependencies generated for this target.
include src/CMakeFiles/ftp_client_lib.dir/depend.make

# Include the progress variables for this target.
include src/CMakeFiles/ftp_client_lib.dir/progress.make

# Include the compile flags for this target's objects.
include src/CMakeFiles/ftp_client_lib.dir/flags.make

src/CMakeFiles/ftp_client_lib.dir/Command.cpp.o: src/CMakeFiles/ftp_client_lib.dir/flags.make
src/CMakeFiles/ftp_client_lib.dir/Command.cpp.o: ../src/Command.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/baothientran/Projects/ftp_client/build-ftp_client-Desktop_Qt_5_11_1_GCC_64bit-Debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/CMakeFiles/ftp_client_lib.dir/Command.cpp.o"
	cd /home/baothientran/Projects/ftp_client/build-ftp_client-Desktop_Qt_5_11_1_GCC_64bit-Debug/src && /usr/bin/x86_64-linux-gnu-g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/ftp_client_lib.dir/Command.cpp.o -c /home/baothientran/Projects/ftp_client/src/Command.cpp

src/CMakeFiles/ftp_client_lib.dir/Command.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ftp_client_lib.dir/Command.cpp.i"
	cd /home/baothientran/Projects/ftp_client/build-ftp_client-Desktop_Qt_5_11_1_GCC_64bit-Debug/src && /usr/bin/x86_64-linux-gnu-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/baothientran/Projects/ftp_client/src/Command.cpp > CMakeFiles/ftp_client_lib.dir/Command.cpp.i

src/CMakeFiles/ftp_client_lib.dir/Command.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ftp_client_lib.dir/Command.cpp.s"
	cd /home/baothientran/Projects/ftp_client/build-ftp_client-Desktop_Qt_5_11_1_GCC_64bit-Debug/src && /usr/bin/x86_64-linux-gnu-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/baothientran/Projects/ftp_client/src/Command.cpp -o CMakeFiles/ftp_client_lib.dir/Command.cpp.s

src/CMakeFiles/ftp_client_lib.dir/Command.cpp.o.requires:

.PHONY : src/CMakeFiles/ftp_client_lib.dir/Command.cpp.o.requires

src/CMakeFiles/ftp_client_lib.dir/Command.cpp.o.provides: src/CMakeFiles/ftp_client_lib.dir/Command.cpp.o.requires
	$(MAKE) -f src/CMakeFiles/ftp_client_lib.dir/build.make src/CMakeFiles/ftp_client_lib.dir/Command.cpp.o.provides.build
.PHONY : src/CMakeFiles/ftp_client_lib.dir/Command.cpp.o.provides

src/CMakeFiles/ftp_client_lib.dir/Command.cpp.o.provides.build: src/CMakeFiles/ftp_client_lib.dir/Command.cpp.o


src/CMakeFiles/ftp_client_lib.dir/FtpService.cpp.o: src/CMakeFiles/ftp_client_lib.dir/flags.make
src/CMakeFiles/ftp_client_lib.dir/FtpService.cpp.o: ../src/FtpService.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/baothientran/Projects/ftp_client/build-ftp_client-Desktop_Qt_5_11_1_GCC_64bit-Debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object src/CMakeFiles/ftp_client_lib.dir/FtpService.cpp.o"
	cd /home/baothientran/Projects/ftp_client/build-ftp_client-Desktop_Qt_5_11_1_GCC_64bit-Debug/src && /usr/bin/x86_64-linux-gnu-g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/ftp_client_lib.dir/FtpService.cpp.o -c /home/baothientran/Projects/ftp_client/src/FtpService.cpp

src/CMakeFiles/ftp_client_lib.dir/FtpService.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ftp_client_lib.dir/FtpService.cpp.i"
	cd /home/baothientran/Projects/ftp_client/build-ftp_client-Desktop_Qt_5_11_1_GCC_64bit-Debug/src && /usr/bin/x86_64-linux-gnu-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/baothientran/Projects/ftp_client/src/FtpService.cpp > CMakeFiles/ftp_client_lib.dir/FtpService.cpp.i

src/CMakeFiles/ftp_client_lib.dir/FtpService.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ftp_client_lib.dir/FtpService.cpp.s"
	cd /home/baothientran/Projects/ftp_client/build-ftp_client-Desktop_Qt_5_11_1_GCC_64bit-Debug/src && /usr/bin/x86_64-linux-gnu-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/baothientran/Projects/ftp_client/src/FtpService.cpp -o CMakeFiles/ftp_client_lib.dir/FtpService.cpp.s

src/CMakeFiles/ftp_client_lib.dir/FtpService.cpp.o.requires:

.PHONY : src/CMakeFiles/ftp_client_lib.dir/FtpService.cpp.o.requires

src/CMakeFiles/ftp_client_lib.dir/FtpService.cpp.o.provides: src/CMakeFiles/ftp_client_lib.dir/FtpService.cpp.o.requires
	$(MAKE) -f src/CMakeFiles/ftp_client_lib.dir/build.make src/CMakeFiles/ftp_client_lib.dir/FtpService.cpp.o.provides.build
.PHONY : src/CMakeFiles/ftp_client_lib.dir/FtpService.cpp.o.provides

src/CMakeFiles/ftp_client_lib.dir/FtpService.cpp.o.provides.build: src/CMakeFiles/ftp_client_lib.dir/FtpService.cpp.o


# Object files for target ftp_client_lib
ftp_client_lib_OBJECTS = \
"CMakeFiles/ftp_client_lib.dir/Command.cpp.o" \
"CMakeFiles/ftp_client_lib.dir/FtpService.cpp.o"

# External object files for target ftp_client_lib
ftp_client_lib_EXTERNAL_OBJECTS =

src/libftp_client_lib.a: src/CMakeFiles/ftp_client_lib.dir/Command.cpp.o
src/libftp_client_lib.a: src/CMakeFiles/ftp_client_lib.dir/FtpService.cpp.o
src/libftp_client_lib.a: src/CMakeFiles/ftp_client_lib.dir/build.make
src/libftp_client_lib.a: src/CMakeFiles/ftp_client_lib.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/baothientran/Projects/ftp_client/build-ftp_client-Desktop_Qt_5_11_1_GCC_64bit-Debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX static library libftp_client_lib.a"
	cd /home/baothientran/Projects/ftp_client/build-ftp_client-Desktop_Qt_5_11_1_GCC_64bit-Debug/src && $(CMAKE_COMMAND) -P CMakeFiles/ftp_client_lib.dir/cmake_clean_target.cmake
	cd /home/baothientran/Projects/ftp_client/build-ftp_client-Desktop_Qt_5_11_1_GCC_64bit-Debug/src && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/ftp_client_lib.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/CMakeFiles/ftp_client_lib.dir/build: src/libftp_client_lib.a

.PHONY : src/CMakeFiles/ftp_client_lib.dir/build

src/CMakeFiles/ftp_client_lib.dir/requires: src/CMakeFiles/ftp_client_lib.dir/Command.cpp.o.requires
src/CMakeFiles/ftp_client_lib.dir/requires: src/CMakeFiles/ftp_client_lib.dir/FtpService.cpp.o.requires

.PHONY : src/CMakeFiles/ftp_client_lib.dir/requires

src/CMakeFiles/ftp_client_lib.dir/clean:
	cd /home/baothientran/Projects/ftp_client/build-ftp_client-Desktop_Qt_5_11_1_GCC_64bit-Debug/src && $(CMAKE_COMMAND) -P CMakeFiles/ftp_client_lib.dir/cmake_clean.cmake
.PHONY : src/CMakeFiles/ftp_client_lib.dir/clean

src/CMakeFiles/ftp_client_lib.dir/depend:
	cd /home/baothientran/Projects/ftp_client/build-ftp_client-Desktop_Qt_5_11_1_GCC_64bit-Debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/baothientran/Projects/ftp_client /home/baothientran/Projects/ftp_client/src /home/baothientran/Projects/ftp_client/build-ftp_client-Desktop_Qt_5_11_1_GCC_64bit-Debug /home/baothientran/Projects/ftp_client/build-ftp_client-Desktop_Qt_5_11_1_GCC_64bit-Debug/src /home/baothientran/Projects/ftp_client/build-ftp_client-Desktop_Qt_5_11_1_GCC_64bit-Debug/src/CMakeFiles/ftp_client_lib.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/CMakeFiles/ftp_client_lib.dir/depend

