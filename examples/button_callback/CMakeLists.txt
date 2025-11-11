# Generated Cmake Pico project file
cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico2 CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(button_callback C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Include the pi_pico_gpio_hal library
set(MODULE_PATH "../..")
add_subdirectory(${MODULE_PATH} libs/pi_pico_gpio_hal)

# Add executable main c file.
add_executable(button_callback button_callback.c)

# Set the name of the application
pico_set_program_name(button_callback "button_led")
pico_set_program_version(button_callback "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(button_callback 1)
pico_enable_stdio_usb(button_callback 1)

# Add the standard library to the build
target_link_libraries(button_callback
                      pico_stdlib)

# Add the standard include files to the build
target_include_directories(button_callback PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

# Add any user requested libraries
target_link_libraries(button_callback 
                      pi_pico_gpio_hal
                      )

# Create the binary that should can be flashed to the pico board
pico_add_extra_outputs(button_callback)