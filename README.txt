
OpenCCL v1.6

/******************************************************************************\

Copyright (c) <2015>, <UNC-CH>
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
may be used to endorse or promote products derived from this software without
specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


---------------------------------
|Please send all BUG REPORTS to:  |
|                                 |
|   sungeui@cs.kaist.ac.kr        |
|   sungeui@gmail.com             |
|                                 |
---------------------------------


The authors may be contacted via:

Mail:         Sung-Eui Yoon
              Dept. of Computer Science, E3-1
              KAIST
              291 Daehak-ro(373-1 Guseong-dong), Yuseong-gu
              DaeJeon, 305-701
              Republic of Korea

\*****************************************************************************/





------------
DIRECTORIES:
------------
libsrc: source for OpenCCL library
include : include files for applications using OpenCCL
example: one example demonstrating the usage of our library
VC_files: Project files for Microsoft Visual Studio 6.0 and 2013
Lib: Contains pre-built libraries for WIN32 and linux


------------
COMPILATION:
------------
0. With CMake (We tested MacOS 10.13 64bit, Ubuntu 16.04 64bit and Windows 10 64bit)
	a) Please install CMake (https://cmake.org/)
	b) From the root directory of this repository, make a folder named 'build'
	c) If OS is MacOS/Linux/Unix, 
		cd build
		cmake ..
		make
	d) Else if OS is Windows,
		using a CMake GUI, set source directory as root directory 
		of this repository and build directory as 'build' folder
		and then 'configure' and 'generate'
		(Please see more on https://cmake.org/runningcmake/)
	e) Runnable example will be generated in build/example/

1. Linux
	a) Please go to "libsrc" directory and type "make"
	b) copy libOpenCCL.a into "Lib" directory
	c) go to "example" directory, type "make", run the Demo 

2. Windows
	a) Please open a project file under "VC_files"
	b) set Demo as a active project and build the demo in the release mode
		- It will automatically copy the library (CCLayout.lib)
		  into Lib directory.

Note for Linux and Windows:
	a) hash_map implementation
	
	My current library uses hash_map.
	However, since hash_map is not a part of STL in VC, I'm using a
	hash_map implementation of Emin Martinian,
	http://www.csua.berkeley.edu/~emin/contact.html

	If you would like to use STL hash_map and have "STLport" in your
	windows development environment, you can uncomment "#define USE_STL_Hashmap"
	at "include/OpenCCL.h".


	b) MeTis library
	
	OpenCCL uses MeTis library for graph partitioning.
	Current distribution has only pre-compiled MeTis library under
	"Lib" directory and a necessary header file under "include/metis"
	directory. If the pre-compiled library does not work properly, please
	download it from 
	   http://www-users.cs.umn.edu/~karypis/metis/metis/index.html
	
	After compilation, please copy the metis library under "Lib" directory.
	
