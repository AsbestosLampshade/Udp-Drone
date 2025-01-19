#ifndef AUXILIARY_FUNCS_H
#define AUXILIARY_FUNCS_H
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "pico/time.h"
#include "hardware/irq.h"
#include "hardware/pwm.h"

#include "lwip/pbuf.h"
#include "lwip/udp.h"

#define SET_BLINK FALSE

extern volatile bool send_gyro_flag;

typedef struct{
    signed short accel[3];
    signed short gyro[3];

}mpu_ret;

extern volatile mpu_ret mpu;
extern volatile mpu_ret mpu_normalized;
extern volatile float battery_voltage;

void logger(char* input,char *type); // Optional, will degrade performance
void pico_led_blink(int count,int delay);//Odd blinks-bad , even blinks-good
void instruction_gather(char* instruction);
void init_pwm();
void set_pwm(int motorno,int power);
void normalize_mpu_data();
void autocorrect();
void timed_normalizer();
void bms_init();
void timed_bms();
#endif