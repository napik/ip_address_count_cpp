cmake_minimum_required(VERSION 3.28.1)
project(ip_counter VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS NO)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O2 -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -mtune=native -ftree-vectorize -flto -funroll-loops -fomit-frame-pointer -DNDEBUG")

set(SOURCES
        FileReader.cpp
        ThreadPool.cpp
        Timer.cpp
        main.cpp
)

add_executable(ip_counter ${SOURCES})

include(CheckIPOSupported)
check_ipo_supported(RESULT result)
if (result)
    set_target_properties(ip_counter PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
endif ()

# Output build type information
message(STATUS "CMake build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "CMake CXX flags (Debug): ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "CMake CXX flags (Release): ${CMAKE_CXX_FLAGS_RELEASE}")