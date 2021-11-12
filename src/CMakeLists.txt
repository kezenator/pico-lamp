cmake_minimum_required(VERSION 3.12)

# Pull in SDK

include(pico_sdk_import.cmake)

# Pull in Pimoroni files.
# NOTE: It doesn't accept environment variables - so add that here

if (DEFINED ENV{PIMORONI_PICO_PATH} AND (NOT PIMORONI_PICO_PATH))
    set(PIMORONI_PICO_PATH $ENV{PIMORONI_PICO_PATH})
    message("Using PIMORONI_PICO_PATH from environment ('${PIMORONI_PICO_PATH}')")
endif ()

include(pimoroni_pico_import.cmake)
add_subdirectory(${PIMORONI_PICO_PATH} pimoroni-pico EXCLUDE_FROM_ALL)

# Start our project setup

project(pico_lamp_project C CXX)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.3.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.3.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

set(PICO_LAMP_PATH ${PROJECT_SOURCE_DIR})

# Initialize the SDK
pico_sdk_init()

add_compile_options(-Wall
        )

add_executable(pico_lamp
        main.cpp
        )

# pull in common dependencies
target_link_libraries(pico_lamp pico_stdlib hardware_spi hardware_pwm hardware_dma pico_display)

# create map/bin/hex file etc.
pico_add_extra_outputs(pico_lamp)