/**
 * @file:       hal_gpio.h
 * @author:     Lucas Wennerholm <lucas.wennerholm@gmail.com>
 * @brief:      Implementation of gpio callback management
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

#include "hal_gpio.h"
#include "pico/stdlib.h"

static halGpioInstance_t inst = {0};

static void gpio_callback(uint gpio, uint32_t event_mask) {
    // Check if the gpio is valid
    if (gpio >= HAL_GPIO_MAX_NUM_INTERFACES) {
        return;
    }

    // Check it the callback is active
    if (inst.list[gpio] == NULL || inst.list[gpio]->data_cb == NULL) {
        return;
    }

    // Check the event
    halGpioEvents_t event;
    if (event_mask & GPIO_IRQ_LEVEL_LOW) {
        event = HAL_GPIO_EVENT_LEVEL_LOW;
    } else if (event_mask & GPIO_IRQ_LEVEL_HIGH) {
        event = HAL_GPIO_EVENT_LEVEL_HIGH;
    } else if (event_mask & GPIO_IRQ_EDGE_FALL) {
        event = HAL_GPIO_EVENT_EDGE_FALL;
    } else if (event_mask & GPIO_IRQ_EDGE_RISE) {
        event = HAL_GPIO_EVENT_EDGE_RISE;
    }

    // Call the registered callback
    inst.list[gpio]->data_cb(inst.list[gpio], event);
}

int32_t halGpioEnableIrqCbRisingEdge(halGpioInterface_t *interface, uint8_t gpio) {
    if (gpio >= HAL_GPIO_MAX_NUM_INTERFACES) {
        return HAL_GPIO_INVALID_ERROR;
    }

    if (interface->data_cb == NULL) {
        return HAL_GPIO_NULL_ERROR;
    }

    inst.list[gpio] = interface;

    // Enable gpio callback
    gpio_set_irq_enabled_with_callback(gpio, GPIO_IRQ_EDGE_RISE, true, gpio_callback);

    return HAL_GPIO_SUCCESS;
}

int32_t halGpioEnableIrqCbFallingEdge(halGpioInterface_t *interface, uint8_t gpio) {
    if (gpio >= HAL_GPIO_MAX_NUM_INTERFACES) {
        return HAL_GPIO_INVALID_ERROR;
    }

    if (interface->data_cb == NULL) {
        return HAL_GPIO_NULL_ERROR;
    }

    inst.list[gpio] = interface;

    // Enable gpio callback
    gpio_set_irq_enabled_with_callback(gpio, GPIO_IRQ_EDGE_FALL, true, gpio_callback);

    return HAL_GPIO_SUCCESS;
}

int32_t halGpioDisableIrqCb(uint8_t gpio) {
    if (gpio >= HAL_GPIO_MAX_NUM_INTERFACES) {
        return HAL_GPIO_INVALID_ERROR;
    }

    // Reset the gpio cb
    inst.list[gpio] = NULL;

    // Disable the IRQ for this GPIO but keep the callback to not affect another GPIO cb
    gpio_set_irq_enabled_with_callback(gpio, 0, false, gpio_callback);

    return HAL_GPIO_SUCCESS;
}

int32_t halGpioInit(void) {
    for (uint32_t i = 0; i < HAL_GPIO_MAX_NUM_INTERFACES; i++) {
        inst.list[i] = NULL;
    }

    gpio_set_irq_callback(gpio_callback);

    return HAL_GPIO_SUCCESS;
}