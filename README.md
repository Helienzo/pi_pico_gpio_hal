# Pi Pico GPIO HAL

Hardware abstraction layer for the Raspberry Pi Pico GPIO library, specifically designed to manage interrupt callbacks with per-pin callback support.

## Overview

The main purpose of this library is to provide **one callback per GPIO pin**, allowing each callback to access its configuration context directly within the interrupt handler. This simplifies GPIO interrupt management compared to the standard Pico SDK, which uses a single global callback for all GPIO interrupts.

## Features

- **Per-Pin Callbacks**: Register individual callbacks for each GPIO pin (up to 28 pins)
- **Edge Detection**: Support for both rising and falling edge interrupts
- **Context Access**: Each callback receives its interface structure, enabling access to application-specific context
- **Simple API**: Minimal wrapper around Pico SDK GPIO functions
- **Helper Functions**: Includes GPIO initialization helper for input pins with pull-up/pull-down configuration

## Requirements

- Raspberry Pi Pico SDK

## Configuration

The library can be configured using preprocessor defines:

- `HAL_GPIO_MAX_NUM_INTERFACES`: Maximum number of GPIO pins (default: 28)

## Usage

For a complete working example demonstrating button interrupt handling with custom context data, see the [examples/button_callback](examples/button_callback) directory.

The example shows how to:
- Initialize the GPIO HAL
- Set up a custom context structure
- Register callbacks for button press events
- Access your custom data within the interrupt handler

## Design Notes

- This library wraps only the IRQ-related GPIO functionality
- Users are expected to handle GPIO pin initialization (or use the provided helper function)
- One callback per GPIO pin limitation is by design for simplicity
- The library uses a static array to store interface pointers for fast lookup in interrupt context
- The `CONTAINER_OF` macro enables accessing custom context data from the base interface pointer

## Status

This library is functional but considered a work in progress. Error handling is minimal in some functions.