#include "auxiliary_funcs.h"

volatile bool send_gyro_flag=false;
volatile mpu_ret mpu;
volatile mpu_ret mpu_normalized;

uint8_t pwmref[4]={0,0,0,0};
uint8_t delta[4]={0,0,0,0}; //Deviations from expected increase/decrease (Presets)

void pico_led_blink(int count,int delay) {

    #if SET_BLINK == TRUE

    for(int i=0;i<count;i++){
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, true);
        sleep_ms(delay);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, false);
        sleep_ms(delay);
    }
    #endif
}

int my_scanner(char* data){
    uint8_t len=0;
    int val=0;
    while(data[len]>='0'&&data[len]<='9'){
        val=val*10;
        val+=data[len]-'0';
        len++;
    }
    return val;
}

void instruction_gather(char* instruction){
    if(memcmp(instruction,"set",3)==0){
        uint8_t motorno,pwm;
        printf("Motor starts");
        pico_led_blink(1,1000);
        printf("\n%s",instruction);
        motorno=my_scanner(&instruction[4]);
        pwm=my_scanner(&instruction[6]);
        printf("\n ::%d %d",motorno,pwm);
        //sscanf(&instruction[4],"%d",&motorno);// %d %d acts weird
        if(motorno>4||motorno<0||pwm>500||pwm<0){
            pwm=0;
            motorno=1;
        }//Protection
        set_pwm(motorno+15,pwm);
        pwmref[motorno-1]=pwm;
    }//Deprecated - automation

    // else if(memcmp(instruction,"y+",2)==0){
    //     //set y+
    //     delta[1]+=5;
    //     delta[2]+=5;
    //     set_pwm(17,pwmref[1]+delta[1]);
    //     set_pwm(18,pwmref[2]+delta[2]);
    // }

    // else if(memcmp(instruction,"y-",2)==0){
    //     //set y+
    //     delta[0]+=5;
    //     delta[3]+=5;
    //     set_pwm(17,pwmref[0]+delta[0]);
    //     set_pwm(18,pwmref[3]+delta[3]);
    // }

    // else if(memcmp(instruction,"z+",2)==0){
    //     //set z+
    //     delta[1]+=5;
    //     delta[0]+=5;
    //     set_pwm(17,pwmref[1]+delta[1]);
    //     set_pwm(18,pwmref[0]+delta[0]);
    // }

    // else if(memcmp(instruction,"z-",2)==0){
    //     //set z-
    //     delta[2]+=5;
    //     delta[3]+=5;
    //     set_pwm(17,pwmref[2]+delta[2]);
    //     set_pwm(18,pwmref[3]+delta[3]);
    // }

    // else if(memcmp(instruction,"x+",2)==0){
    //     //set z+
    //     delta[1]+=5;
    //     delta[3]+=5;
    //     set_pwm(17,pwmref[1]+delta[1]);
    //     set_pwm(18,pwmref[3]+delta[3]);
    // }

    // else if(memcmp(instruction,"z-",2)==0){
    //     //set z-
    //     delta[0]+=5;
    //     delta[2]+=5;
    //     set_pwm(17,pwmref[0]+delta[0]);
    //     set_pwm(18,pwmref[2]+delta[2]);
    // }

    else if(memcmp(instruction,"disable",7)==0){
        send_gyro_flag=false;
    }
    else if(memcmp(instruction,"enable",5)==0){
        send_gyro_flag=true;
    }

    // else if(memcmp(instruction,"up",2)==0){
    //     if(pwmref[0]>250||pwmref[1]>250||pwmref[2]>250||pwmref[3]>250){
    //         pico_led_blink(7,200);
    //         return;
    //     }
    //     pwmref[0]+=5;
    //     pwmref[1]+=5;
    //     pwmref[2]+=5;
    //     pwmref[3]+=5;
    //     set_pwm(16,pwmref[0]+delta[0]);
    //     set_pwm(17,pwmref[1]+delta[1]);
    //     set_pwm(18,pwmref[2]+delta[2]);
    //     set_pwm(19,pwmref[3]+delta[3]);
    // }

    // else if(memcmp(instruction,"down",4)==0){
    //     if(pwmref[0]<5||pwmref[1]<5||pwmref[2]<5||pwmref[3]<5){
    //         pico_led_blink(7,200);
    //         return;
    //     }
    //     pwmref[0]-=5;
    //     pwmref[1]-=5;
    //     pwmref[2]-=5;
    //     pwmref[3]-=5;
    //     set_pwm(16,pwmref[0]+delta[0]);
    //     set_pwm(17,pwmref[1]+delta[1]);
    //     set_pwm(18,pwmref[2]+delta[2]);
    //     set_pwm(19,pwmref[3]+delta[3]);
    // }

    // else if(memcmp(instruction,"delta",5)==0){
    //     uint8_t motorno;
    //     int shift;
    //     sscanf(&instruction[6],"%d %d",&motorno,&shift);
    //     if(delta[motorno-1]+shift<0){
    //         printf("Negative,  no change");
    //         pico_led_blink(5,100);
    //         return;
    //     }
    //     delta[motorno-1]+=shift;
    // }

    else if(memcmp(instruction,"all",3)==0){
        uint8_t pwm;
        sscanf(&instruction[4],"%d",&pwm);
        if(pwm>500||pwm<0){
            pwm=0;
        }//Protection
        memset(pwmref,pwm,4);
        set_pwm(16,pwm+delta[0]);
        set_pwm(17,pwm+delta[1]);
        set_pwm(18,pwm+delta[2]);
        set_pwm(19,pwm+delta[3]);
    }

    else if(memcmp(instruction,"dyn",3)==0){
        uint8_t pwm1,pwm2,pwm3,pwm4;
        printf("dyn entered\n");
        //sscanf(&instruction[4],"%d %d %d %d",&pwm1,&pwm2,&pwm3,&pwm4);
        pwm1=my_scanner(&instruction[4]);
        pwm2=my_scanner(&instruction[8]);
        pwm3=my_scanner(&instruction[12]);
        pwm4=my_scanner(&instruction[16]);
        printf("%d %d %d %d\n",pwm1,pwm2,pwm3,pwm4);
        if(pwm1>500||pwm1<0||pwm2>500||pwm3>500||pwm4>500||pwm2<0||pwm3<0||pwm4<0){
            pwm1=0;
            pwm2=0;
            pwm3=0;
            pwm4=0;
        }//Protection
        // pwmref[0]=pwm1;
        // pwmref[1]=pwm2;
        // pwmref[2]=pwm3;
        // pwmref[3]=pwm4;
        // set_pwm(16,pwm1+delta[0]);
        // set_pwm(17,pwm2+delta[1]);
        // set_pwm(18,pwm3+delta[2]);
        // set_pwm(19,pwm4+delta[3]);
        set_pwm(16,pwm1);
        set_pwm(17,pwm2);
        set_pwm(18,pwm3);
        set_pwm(19,pwm4);
    }

    else if(memcmp(instruction,"exit",4)<=0){
        printf("exit");
        set_pwm(16,0);
        set_pwm(17,0);
        set_pwm(18,0);
        set_pwm(19,0);
        memset(pwmref,0,4);
        //put if check outside
    }

    printf("Instructions complete\n");

}

//on call setup/init pwm
void init_pwm(){
 // Tell the LED pin that the PWM is in charge of its value.
    gpio_set_function(16, GPIO_FUNC_PWM);
    gpio_set_function(17, GPIO_FUNC_PWM);
    gpio_set_function(18, GPIO_FUNC_PWM);
    gpio_set_function(19, GPIO_FUNC_PWM);
    // Figure out which slice we just connected to the LED pin
    uint slice_num = pwm_gpio_to_slice_num(16);
    uint slice_num_2 = pwm_gpio_to_slice_num(18);

    // Get some sensible defaults for the slice configuration. By default, the
    // counter is allowed to wrap over its maximum range (0 to 2**16-1)
    pwm_config config = pwm_get_default_config();
    // Set divider, reduces counter clock to sysclock/this value
    pwm_config_set_clkdiv_int(&config, 10);
    //Base clock to pwm clock
    pwm_config_set_wrap(&config,500);
    // Load the configuration into our PWM slice, and set it running.
    pwm_init(slice_num, &config, true);
    pwm_init(slice_num_2, &config, true);
    pwm_set_gpio_level(16,0);
    pwm_set_gpio_level(17,0);
    pwm_set_gpio_level(18,0);
    pwm_set_gpio_level(19,0);
}

// on call increase/decrease pwm value
void set_pwm(int motorno,int power){
    pwm_set_gpio_level(motorno, power);
}

void normalize_mpu_data(){
    static int count=0;
    static int accel_x=0;
    static int accel_y=0;
    static int accel_z=0;
    static int gyro_x=0;
    static int gyro_y=0;
    static int gyro_z=0;
    while(count<100){
        //printf("Creating average");
        accel_x+=mpu.accel[0];
        accel_y+=mpu.accel[1];
        accel_z+=mpu.accel[2];
        gyro_x+=mpu.gyro[0];
        gyro_y+=mpu.gyro[1];
        gyro_z+=mpu.gyro[2];
        count++;
        if(count==100){
            //send_gyro_flag=true;
            printf("Normalization Complete\n");
            accel_x/=100;
            accel_y/=100;
            accel_z/=100;
            gyro_x/=100;
            gyro_y/=100;
            gyro_z/=100;
            //printf("%d %d %d\n",accel_x,accel_y,accel_z);
        }
        return;
    } 
    //printf("Subtracting");
    mpu_normalized.accel[0]=mpu.accel[0]-accel_x;
    mpu_normalized.accel[1]=mpu.accel[1]-accel_y;
    mpu_normalized.accel[2]=mpu.accel[2]-accel_z;
    mpu_normalized.gyro[0]=mpu.gyro[0]-gyro_x;
    mpu_normalized.gyro[1]=mpu.gyro[1]-gyro_y;
    mpu_normalized.gyro[2]=mpu.gyro[2]-gyro_z;
    //autocorrect();
    //printf("\n%d %d %d\n",mpu_normalized.accel[0],mpu_normalized.accel[1],mpu_normalized.accel[2]);
}


#define minimum_correction 1310
void autocorrect(){
    // 1 2
    // 4 3
    static int localchange[4]={0,0,0,0};
    //Gyration is RHT rule
    //On x axis gyration -> Points upward

    //On y axis gyration -> Points south
    if(mpu_normalized.gyro[1]<-minimum_correction&&localchange[1]<30&&localchange[2]<30){
        localchange[1]+=5;
        localchange[2]+=5;
    }
    if(mpu_normalized.gyro[1]>minimum_correction&&localchange[1]>-30&&localchange[2]>-30){
        localchange[1]-=5;
        localchange[2]-=5;
    }
    //On z axis gyration -> Points east
    if(mpu_normalized.gyro[2]<-minimum_correction&&localchange[1]<30&&localchange[0]<30){
        localchange[0]+=5;
        localchange[1]+=5;
    }
    if(mpu_normalized.gyro[2]>minimum_correction&&localchange[1]>-30&&localchange[0]>-30){
        localchange[0]-=5;
        localchange[1]-=5;
    }

    //Using inputs
    set_pwm(16,pwmref[0]+delta[0]+localchange[0]);
    set_pwm(17,pwmref[1]+delta[1]+localchange[1]);
    set_pwm(18,pwmref[2]+delta[2]+localchange[2]);
    set_pwm(19,pwmref[3]+delta[3]+localchange[3]);
    printf("%d %d %d %d\n",localchange[0],localchange[1],localchange[2],localchange[3]);
}

bool timer_callback_norm(__unused struct repeating_timer *t){
	normalize_mpu_data();
    return true;
}

void timed_normalizer(){
	static struct repeating_timer timer;
	if(add_repeating_timer_ms(-20, timer_callback_norm, NULL, &timer)){//Make this a define
		printf("Timer Added Successfully\n");
		return;
	}
	printf("Timer Could not be Added\n");
	return;
}

void bms_init(){
    adc_init();
	// Make sure GPIO is high-impedance, no pullups etc
	adc_gpio_init(26);
	// Select ADC input 0 (GPIO26)
	adc_select_input(0);
}

void timer_callback_bms(){
    const float conversion_factor = 3.3f / (1 << 12);
    uint16_t result = adc_read();
    printf("Raw value: 0x%03x, voltage: %f V\n", result, result * conversion_factor);
    battery_voltage=result;
}

void timed_bms(){
    static struct repeating_timer timer;
	if(add_repeating_timer_ms(-20, timer_callback_bms, NULL, &timer)){
		printf("Timer Added Successfully\n");
		return;
	}
	printf("Timer Could not be Added\n");
	return;
}