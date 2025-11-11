#include <stdio.h>
#include "pico/stdlib.h"
#include "hal_gpio.h"

// Create a LOG function that can be used to print over UART
#ifndef LOG
#define LOG(f_, ...) printf((f_), ##__VA_ARGS__)
#endif

// Use a custom GPIO as LED
#define CUSTOM_LED_GPIO (13)
#define BUTTON_GPIO_1   (7)
#define BUTTON_GPIO_2   (8)

static void device_error();

// Interrupt events
typedef enum {
    MAIN_GPIO_IDLE,
    MAIN_GPIO_1_RISING_EDGE_EVENT,
    MAIN_GPIO_1_FALLING_EDGE_EVENT,
    MAIN_GPIO_2_RISING_EDGE_EVENT,
    MAIN_GPIO_2_FALLING_EDGE_EVENT,
} gpioIntEvents_t;

// Declare the myInstance type
typedef struct {
   // GPIO management
    halGpioInterface_t gpio_1_interface;
    halGpioInterface_t gpio_2_interface;
    volatile uint32_t  interrupt_event;
} myInstance_t;

// Create one instance of myInstance and initialize it to zeros
static myInstance_t my_instance = {0};

// Turn the led on or off
void pico_set_led(bool led_on) {
    // Just set the GPIO on or off
    gpio_put(PICO_DEFAULT_LED_PIN, led_on);
}

// Turn the led on or off
void pico_set_custom_led(bool led_on) {
    // Just set the GPIO on or off
    gpio_put(CUSTOM_LED_GPIO, led_on);
}

// Perform initialisation
int pico_led_init(void) {
    // A device like Pico that uses a GPIO for the LED will define PICO_DEFAULT_LED_PIN
    // so we can use normal GPIO functionality to turn the led on and off
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    gpio_init(CUSTOM_LED_GPIO);
    gpio_set_dir(CUSTOM_LED_GPIO, GPIO_OUT);
    return PICO_OK;
}

// THIS IS IN IRQ CONTEXT!
void gpio1EdgeCb(halGpioInterface_t *interface, halGpioEvents_t event) {
    // Get the instance pointer from the interface
    myInstance_t * inst = CONTAINER_OF(interface, myInstance_t, gpio_1_interface);

    switch (event) {
    case HAL_GPIO_EVENT_EDGE_FALL:
        inst->interrupt_event = MAIN_GPIO_1_FALLING_EDGE_EVENT;
        pico_set_led(true);
        break;
    case HAL_GPIO_EVENT_EDGE_RISE:
        inst->interrupt_event = MAIN_GPIO_1_RISING_EDGE_EVENT;
        pico_set_led(false);
        break;
    default:
        break;
    }
}

// THIS IS IN IRQ CONTEXT!
void gpio2EdgeCb(halGpioInterface_t *interface, halGpioEvents_t event) {
    // Get the instance pointer from the interface
    myInstance_t * inst = CONTAINER_OF(interface, myInstance_t, gpio_2_interface);

    switch (event) {
    case HAL_GPIO_EVENT_EDGE_FALL:
        inst->interrupt_event = MAIN_GPIO_2_FALLING_EDGE_EVENT;
        pico_set_custom_led(true);
        break;
    case HAL_GPIO_EVENT_EDGE_RISE:
        inst->interrupt_event = MAIN_GPIO_2_RISING_EDGE_EVENT;
        pico_set_custom_led(false);
        break;
    default:
        break;
    }
}

int main()
{
    // Init the sdk, enable UART and USB printf
    stdio_init_all();

    // Init the leds
    int rc = pico_led_init();
    hard_assert(rc == PICO_OK);

    // Initialize the gpio module
    halGpioInit();

    // Configure GPIOs pin as input with pull down
    halGpioInitInputPin(BUTTON_GPIO_1, true, true);
    halGpioInitInputPin(BUTTON_GPIO_2, true, true);

    // Configure the gpio callbacks
    my_instance.interrupt_event = MAIN_GPIO_IDLE;
    my_instance.gpio_1_interface.data_cb = gpio1EdgeCb;
    my_instance.gpio_2_interface.data_cb = gpio2EdgeCb;

    // Enable falling edge events for the first GPIO
    if (halGpioEnableIrqCbFallingEdge(&my_instance.gpio_1_interface, BUTTON_GPIO_1) != HAL_GPIO_SUCCESS) {
        device_error();
    }

    // Enable rising edge events for the first GPIO
    if (halGpioEnableIrqCbRisingEdge(&my_instance.gpio_1_interface, BUTTON_GPIO_1) != HAL_GPIO_SUCCESS) {
        device_error();
    }

    // Enable falling edge events for the second GPIO
    if (halGpioEnableIrqCbFallingEdge(&my_instance.gpio_2_interface, BUTTON_GPIO_2) != HAL_GPIO_SUCCESS) {
        device_error();
    }

    // Enable rising edge events for the second GPIO
    if (halGpioEnableIrqCbRisingEdge(&my_instance.gpio_2_interface, BUTTON_GPIO_2) != HAL_GPIO_SUCCESS) {
        device_error();
    }

    while (true) {
        // Manage interrupts
        switch (my_instance.interrupt_event) {
            case MAIN_GPIO_IDLE:
                break;
            case MAIN_GPIO_1_RISING_EDGE_EVENT:
                // Reset the event
                my_instance.interrupt_event = MAIN_GPIO_IDLE;
                LOG("BUTTON 1 PRESSED!\n");
                break;
            case MAIN_GPIO_1_FALLING_EDGE_EVENT:
                // Reset the event
                my_instance.interrupt_event = MAIN_GPIO_IDLE;
                LOG("BUTTON 1 RELEASED!\n");
                break;
            case MAIN_GPIO_2_RISING_EDGE_EVENT:
                // Reset the event
                my_instance.interrupt_event = MAIN_GPIO_IDLE;
                LOG("BUTTON 2 PRESSED!\n");
                break;
            case MAIN_GPIO_2_FALLING_EDGE_EVENT:
                // Reset the event
                my_instance.interrupt_event = MAIN_GPIO_IDLE;
                LOG("BUTTON 2 RELEASED!\n");
                break;
            default:
               // This is invalid!
               device_error();
               break;
        }
    }
}

static void device_error() {
    // Forever blink fast
    while (true) {
        pico_set_led(true);
        sleep_ms(100);
        pico_set_led(false);
        sleep_ms(100);
    }
}