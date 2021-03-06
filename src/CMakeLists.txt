cmake_minimum_required(VERSION 2.6)
project(TRENCHER C CXX)

#set(CMAKE_VERBOSE_MAKEFILE ON)

# Generate compilation database for YouCompleteMe.
set(CMAKE_EXPORT_COMPILE_COMMANDS YES)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif(NOT CMAKE_BUILD_TYPE)

set(ENABLE_PROFILING OFF CACHE BOOL "Enable profiling")

if(CMAKE_COMPILER_IS_GNUCXX)
	add_definitions(-fno-strict-aliasing)
	add_definitions(-Wall -Wextra)
	add_definitions(-fshow-column)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -std=c++1y")

	if(ENABLE_PROFILING)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
	endif()
endif()

if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")
endif()

find_package(Threads)

if(WIN32)
	set(Boost_USE_STATIC_LIBS ON)
	add_definitions(-DBOOST_THREAD_USE_LIB)
endif()
find_package(Boost 1.47.0 REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

include_directories(${TRENCHER_SOURCE_DIR})

add_subdirectory(trench)
add_subdirectory(trencher)
