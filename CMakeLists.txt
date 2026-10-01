cmake_minimum_required(VERSION 3.13)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(kestrel_perch C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(kestrel_perch
    src/main.c
    src/usb_descriptors.c
    src/uart.c
    src/power.c
    src/command.c
    src/pio_uart.c
)

# Compila o arquivo PIO
pico_generate_pio_header(kestrel_perch ${CMAKE_CURRENT_LIST_DIR}/pio/uart_rx.pio)
pico_generate_pio_header(kestrel_perch ${CMAKE_CURRENT_LIST_DIR}/pio/uart_tx.pio)

target_include_directories(kestrel_perch PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

# Bibliotecas do Pico SDK
target_link_libraries(kestrel_perch
    pico_stdlib
    hardware_uart
    hardware_pio
    hardware_flash
    hardware_sync
    pico_multicore
    tinyusb_device
    tinyusb_board
)

target_include_directories(kestrel_perch PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src/include
    ${CMAKE_CURRENT_LIST_DIR}/include
)

# Desativa a stdio padrão para podermos gerenciar o TinyUSB manualmente
pico_enable_stdio_usb(kestrel_perch 0)
pico_enable_stdio_uart(kestrel_perch 0)

pico_add_extra_outputs(kestrel_perch)