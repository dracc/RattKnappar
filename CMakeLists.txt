cmake_minimum_required(VERSION 3.20)

# initialize pico-sdk from GIT
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_FETCH_FROM_GIT on)

# Set board type
# note: this must happen before the pico_sdk_import include
set(PICO_BOARD waveshare_rp2040_zero)
set(CMAKE_BUILD_TYPE Release)

# pico_sdk_import.cmake is a single file copied from this SDK
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(RattKnappar)

#set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

include_directories(include)

# Left button module
add_executable(left_module
  src/modules.cpp
)

target_compile_options(left_module PUBLIC -Wall -Wextra -pedantic -std=gnu++11)

target_compile_definitions(left_module
  PUBLIC __LEFT
)

target_link_libraries(left_module
  pico_i2c_slave
  hardware_i2c
  pico_stdlib
)

pico_add_extra_outputs(left_module)

# Right button module
add_executable(right_module
  src/modules.cpp
)

target_compile_definitions(right_module
  PUBLIC __RIGHT
)

target_link_libraries(right_module
  pico_i2c_slave
  hardware_i2c
  pico_stdlib
)

pico_add_extra_outputs(right_module)
