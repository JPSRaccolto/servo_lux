# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)


set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(servo_lux C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(servo_lux servo_lux.c lib/bh1750_light_sensor.c lib/ssd1306.c)

pico_set_program_name(servo_lux "servo_lux")
pico_set_program_version(servo_lux "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(servo_lux 1)
pico_enable_stdio_usb(servo_lux 1)

# Add the standard library to the build
target_link_libraries(servo_lux
        pico_stdlib
        hardware_i2c)

# Add the standard include files to the build
target_include_directories(servo_lux PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/lib
)



pico_add_extra_outputs(servo_lux)

