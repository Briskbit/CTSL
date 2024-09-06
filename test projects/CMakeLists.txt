cmake_minimum_required(VERSION 3.14)


set(STDC23 "NO")  #YES NO


################################# C TEST ##################################################

project(c_test C)

if (STDC23 STREQUAL "YES")
	set(CMAKE_C_STANDARD 23)
	add_definitions(-DSTDC23=1) 
else()
	set(CMAKE_C_STANDARD 11)
	add_definitions(-DDEFINE_BOOL=1) 
endif()

add_executable(${PROJECT_NAME}
        c_test.c
)

if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    set(BUILD_SHARED_LIBS OFF)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -Wall -Wno-unknown-pragmas")
    set(CMAKE_C_FLAGS_RELEASE "-O3")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	add_definitions(-D_CRT_SECURE_NO_WARNINGS=1)
	target_compile_options(${PROJECT_NAME} PRIVATE "/DNDEBUG /MT /O2 /Ot /MACHINE:X64") 
endif()

if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
	target_include_directories(${PROJECT_NAME} PRIVATE
	   /usr/include/ctsl/
	)
	target_link_directories(${PROJECT_NAME} PRIVATE
	#   /path/to/ctsl/lib/
	) 
	target_link_libraries(${PROJECT_NAME} PRIVATE
		libctsl.so
	)
elseif (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
	target_include_directories(${PROJECT_NAME} PRIVATE
	#   /path/to/ctsl/include/
	)
	target_link_directories(${PROJECT_NAME} PRIVATE
	#   /path/to/ctsl/lib/
	) 
	target_link_libraries(${PROJECT_NAME} PRIVATE
			ctsl.lib
	)
endif()


################################# CPP TEST ##################################################

project(cpp_test CXX)

if (STDC23 STREQUAL "YES")
	set(CMAKE_CXX_STANDARD 23)
	add_definitions(-DSTDC23=1) 
else()
	set(CMAKE_CXX_STANDARD 20)
endif()

add_executable(${PROJECT_NAME}
    cpp_test.cpp
)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(BUILD_SHARED_LIBS OFF)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -Wall -Wno-unknown-pragmas")
    set(CMAKE_CXX_FLAGS_DEBUG   "-O0 -g3")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
  # using Intel C++
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	add_definitions(-D_CRT_SECURE_NO_WARNINGS=1)
	target_compile_options(${PROJECT_NAME} PRIVATE "/DNDEBUG /MT /O2 /Ot /MACHINE:X64" ) 
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_include_directories(${PROJECT_NAME} PRIVATE
	   /usr/include/ctsl/
	)
	target_link_directories(${PROJECT_NAME} PRIVATE
	#   /path/to/ctsl/lib/
	) 
	target_link_libraries(${PROJECT_NAME} PRIVATE
		libctsl.so
	)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	target_include_directories(${PROJECT_NAME} PRIVATE
	#   /path/to/ctsl/include/
	)
	target_link_directories(${PROJECT_NAME} PRIVATE
	#   /path/to/ctsl/lib/
	) 
	target_link_libraries(${PROJECT_NAME} PRIVATE
		ctsl.lib
	)
endif()
