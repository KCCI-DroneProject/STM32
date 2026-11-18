cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME KCCI-Drone)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# K-Drone 소스 폴더 목록 — 폴더를 새로 만들면 여기에 추가한다.
# CONFIGURE_DEPENDS: 폴더 안에 파일을 추가·삭제하면 다음 빌드 때 자동으로 재구성된다.
file(GLOB KDRONE_SOURCES CONFIGURE_DEPENDS
    K-Drone/App/Src/*.c
    K-Drone/Drivers/Src/*.c
    K-Drone/Utils/Src/*.c
)

# K-Drone 헤더 폴더 목록 — 폴더를 새로 만들면 여기에 추가한다.
set(KDRONE_INCLUDE_DIRS
    K-Drone/App/Inc
    K-Drone/Config
    K-Drone/Utils/Inc
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
    ${KDRONE_SOURCES}
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
    ${KDRONE_INCLUDE_DIRS}
)

# 미리 컴파일할 표준 헤더 (모든 C 소스에 자동 포함된다)
target_precompile_headers(${CMAKE_PROJECT_NAME} PRIVATE
    <string.h>
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)
