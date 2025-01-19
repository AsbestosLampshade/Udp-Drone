/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

// Fade an LED between low and high brightness. An interrupt handler updates
// the PWM slice's output level each time the counter wraps.

#include "pico/stdlib.h"
#include <stdio.h>
#include "pico/time.h"
#include "hardware/irq.h"
#include "hardware/pwm.h"

//on call setup/init pwm
void init_pwm(){
 // Tell the LED pin that the PWM is in charge of its value.
    gpio_set_function(16, GPIO_FUNC_PWM);
    // Figure out which slice we just connected to the LED pin
    uint slice_num = pwm_gpio_to_slice_num(16);

    // Get some sensible defaults for the slice configuration. By default, the
    // counter is allowed to wrap over its maximum range (0 to 2**16-1)
    pwm_config config = pwm_get_default_config();
    // Set divider, reduces counter clock to sysclock/this value
    pwm_config_set_clkdiv_int(&config, 250);
    //Base clock to pwm clock
    pwm_config_set_wrap(&config,250);
    // Load the configuration into our PWM slice, and set it running.
    pwm_init(slice_num, &config, true);
}

int main() {
    init_pwm();
    pwm_set_gpio_level(16, 200);
    sleep_ms(1000);
    pwm_set_gpio_level(16, 20);
    while (1)
        tight_loop_contents();
}
