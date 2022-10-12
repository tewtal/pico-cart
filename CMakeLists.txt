cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(picocart_test C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()


add_executable(picocart pico-cart.c)
pico_generate_pio_header(picocart ${CMAKE_CURRENT_LIST_DIR}/pico-cart.pio)
target_link_libraries(picocart pico_stdlib hardware_pio)

pico_enable_stdio_usb(picocart 1)
pico_enable_stdio_uart(picocart 0)

pico_add_extra_outputs(picocart)

pico_define_boot_stage2(slower_boot2 ${PICO_DEFAULT_BOOT_STAGE2_FILE})
target_compile_definitions(slower_boot2 PRIVATE PICO_FLASH_SPI_CLKDIV=6)

pico_set_boot_stage2(picocart slower_boot2)