cmake_minimum_required(VERSION 3.13)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include("$ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake")
include("$ENV{PICO_EXTRAS_PATH}/external/pico_extras_import.cmake")

project(kestrel_perch)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

SET(FREERTOS_KERNEL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib/FreeRTOS-Kernel)
include(${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake)

pico_sdk_init()

add_executable(kestrel_perch
    src/main.c
    )

# pull in common dependencies
target_link_libraries(kestrel_perch PUBLIC
    pico_stdlib
    pico_multicore
)

target_include_directories(pico_audio_i2s_32b INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/src/include
)

pico_enable_stdio_usb(kestrel_perch 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(kestrel_perch)