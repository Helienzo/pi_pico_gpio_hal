cmake_minimum_required(VERSION 3.13)

project(pi_pico_gpio_hal C ASM)

add_library(pi_pico_gpio_hal INTERFACE)

target_sources(pi_pico_gpio_hal INTERFACE
	src/hal_gpio.c
)

target_include_directories(pi_pico_gpio_hal INTERFACE
	src
)