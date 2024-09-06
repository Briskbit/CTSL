
# CTSL  (C Tools Library)  v. 1.0
	
CTSL is a fast and lightweight library that contains STL-like containers (e.g. vectors, maps, etc. The full list is available in ctsl.h) and is primarily aimed at C developers. But it can be useful for C++ developers as well, since it also contains Python-like containers (e.g. list, dictionary, queue, which can hold arbitrary data types in a single container). The library also contains rich methods for manipulating ANSI and UNICODE strings.

## Installation

All installation packages are located in the 'Distribution' folder.

### Linux
***NOTE***: *glibc version must be >= 2.28*
Linux distribution packages are located in the "Linux" subfolder (you need to select the appropriate package for the OS).  After unpacking the tar package into the target folder (`tar -xf ctls-1.0-xxx.tar.xz`) you can optionally run the ctsl-1.0/install.sh script with root privileges, which will register the library under `usr` for easy access to library components in the future.

### Windows
Windows distribution packages are located in the "Windows" subfolder. Unzip package into the target folder. Optionally the ctsl/x64/install.bat can be run to include x64 folder to PATH.

### MacOS
The installation package will be provided shortly.

## Examples
The 'test projects' subfolder contains C and C++ projects with the examples for each primitive from the ctsl library. To build the projects first fill the 'target_include_directories' and 'target_link_directories' (CMakeLists.txt)  with the paths to 'include' and 'lib'(LINUX)/'x64' (WINDOWS) folders.   **NOTE**: *to build projects with C/C++ 23 standard please set STDC23 to YES in CMakeLists.txt.*
