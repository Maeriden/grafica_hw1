cmake_minimum_required (VERSION 3.5)

project (graphics17a_hw01)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_BUILD_TYPE Debug)
#set(CMAKE_BUILD_TYPE Release)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# compile flags
if(APPLE)
    add_definitions(-Wno-missing-braces)
endif(APPLE)
if(WIN32)
    add_definitions(/D_CRT_SECURE_NO_WARNINGS /wd4018 /wd4244 /wd4305 /wd4800 /wd4267)
endif(WIN32)
if(LINUX)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DENABLE_ASSERT=1")
endif(LINUX)

add_definitions(-DYOBJ_NO_IMAGE -DYGLTF_NO_IMAGE)

add_library(hwlib STATIC src/image.h src/image.cpp src/vmath.h
    src/yocto_utils.h src/stb_image.h src/stb_image_write.h)

add_executable(tonemap src/tonemap.cpp)
add_executable(compose src/compose.cpp)

target_link_libraries(tonemap hwlib)
target_link_libraries(compose hwlib)
