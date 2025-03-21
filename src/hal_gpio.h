/**
 * @file:       hal_gpio.c
 * @author:     Lucas Wennerholm <lucas.wennerholm@gmail.com>
 * @brief:      Header file for GPIO management
 *
 * @license: MIT License
 *
 * Copyright (c) 2024 Lucas Wennerholm
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

#ifndef HAL_GPIO_H
#define HAL_GPIO_H
#include "pico/stdlib.h"

/**
 * This module wraps the gpio functionality to enable one callback per event and GPIO
 * Note that the module is limited to using one callback per gpio
 * To avoid wrapping all functionality only IRQ related functionality is wrapped
 * The user of this module is expected to do all initialization of the GPIO.
 */

#ifndef HAL_GPIO_MAX_NUM_INTERFACES
#define HAL_GPIO_MAX_NUM_INTERFACES (28)
#endif /* HAL_GPIO_MAX_NUM_INTERFACES */

typedef enum {
    HAL_GPIO_SUCCESS,
    HAL_GPIO_NULL_ERROR = -1,
    HAL_GPIO_INVALID_ERROR = -2,
} halGpioErr_t;

typedef enum {
    HAL_GPIO_EVENT_LEVEL_LOW,
    HAL_GPIO_EVENT_LEVEL_HIGH,
    HAL_GPIO_EVENT_EDGE_FALL,
    HAL_GPIO_EVENT_EDGE_RISE,
} halGpioEvents_t;

typedef struct halGpioInterface halGpioInterface_t; // Forward declararion of upstream interface
typedef void (*halGpioEventCallback_t)(halGpioInterface_t *interface, halGpioEvents_t event);

// Definition of interface
struct halGpioInterface {
    halGpioEventCallback_t data_cb;
};

typedef struct {
    // Callback management
    halGpioInterface_t *list[HAL_GPIO_MAX_NUM_INTERFACES];
} halGpioInstance_t;

/**
 * Enable a callback on a specific GPIO on rising edge
 * Input: Pointer to an interface structure, or null do deinit an IRQ
 * Input: gpio pin number
 * Returns: halGpioErr_t
 */
int32_t halGpioEnableIrqCbRisingEdge(halGpioInterface_t *interface, uint8_t gpio);

/**
 * Enable a callback on a specific GPIO on falling edge
 * Input: Pointer to an interface structure, or null do deinit an IRQ
 * Input: gpio pin number
 * Returns: halGpioErr_t
 */
int32_t halGpioEnableIrqCbFallingEdge(halGpioInterface_t *interface, uint8_t gpio);

/**
 * Disable an active IRQ
 * Input: gpio pin number
 * Returns: halGpioErr_t
 */
int32_t halGpioDisableIrqCb(uint8_t gpio);

/**
 * Initialize this module, configures the lower layer gpio callback
 */
int32_t halGpioInit(void);

#endif /* HAL_GPIO_H */