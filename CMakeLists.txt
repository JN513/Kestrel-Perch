cmake_minimum_required(VERSION 3.13)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(pico_serial_bridge C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(pico_serial_bridge
    src/main.c
    src/usb_descriptors.c
)

# Compila o arquivo PIO
pico_generate_pio_header(pico_serial_bridge ${CMAKE_CURRENT_LIST_DIR}/src/uart_tx_rx.pio)

target_include_directories(pico_serial_bridge PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

# Bibliotecas do Pico SDK
target_link_libraries(pico_serial_bridge
    pico_stdlib
    hardware_uart
    hardware_pio
    hardware_flash
    hardware_sync
    pico_multicore
    tinyusb_device
    tinyusb_board
)

target_include_directories(pico_serial_bridge PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src/include
    ${CMAKE_CURRENT_LIST_DIR}/include
)

# Desativa a stdio padrão para podermos gerenciar o TinyUSB manualmente
pico_enable_stdio_usb(pico_serial_bridge 0)
pico_enable_stdio_uart(pico_serial_bridge 0)

pico_add_extra_outputs(pico_serial_bridge)