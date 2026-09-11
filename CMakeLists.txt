cmake_minimum_required(VERSION 3.13)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include("$ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake")
include("$ENV{PICO_EXTRAS_PATH}/external/pico_extras_import.cmake")

project(kestrel_perch)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()


add_executable(kestrel_perch
    src/main.c
    src/perch.c
    src/current.c
    src/device.c
    src/usb.c
    src/power.c
    src/protocol.c
    src/uart.c
    )

# pull in common dependencies
target_link_libraries(kestrel_perch PUBLIC
    pico_stdlib
    pico_multicore
    hardware_adc
    hardware_pio
    hardware_dma
    hardware_clocks
    tinyusb_device
    tinyusb_board
)

target_include_directories(kestrel_perch PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src/include
    ${CMAKE_CURRENT_LIST_DIR}/include
)

# pico_enable_stdio_usb(kestrel_perch 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(kestrel_perch)