#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"

//PLEASE ENABLE DLPF
//Please check for error

#define CLK_GPIO 5
#define DATA_GPIO 4
#define MPU_ADDR 0x68
#define WHO_AM_I_REG	0x75
#define TEMP_MSB 0x41
#define TEMP_LSB 0x42
#define PWR_MGMT0 0x6B
#define AFS 0x1C
#define FS 0x1B
#define GYRO 0x43
#define ACCEL 0x3B

void i2c_begin(){
		i2c_init(i2c_default,100*1000);
		gpio_set_function(CLK_GPIO,GPIO_FUNC_I2C);
		gpio_set_function(DATA_GPIO,GPIO_FUNC_I2C);
		gpio_pull_up(CLK_GPIO);
		gpio_pull_up(DATA_GPIO);
		printf("\nreached here\n");
		// Make the I2C pins available to picotool
		bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));
		uint8_t data = 0x75;
		int ret = i2c_write_timeout_us(i2c_default,MPU_ADDR,&data,sizeof(data),false,5000);
		printf("%d,write\n",ret);
		ret = i2c_write_timeout_us(i2c_default,MPU_ADDR,&data,sizeof(data),false,5000);
		printf("\n%d,read",ret);
		printf("\n%02X",data);
}

void i2c_accel(){

}

void i2c_gyro(){

}

int main(){
	int retVal;
	signed short s;
	float temp;
	signed short gyro_x,gyro_y,gyro_z,accel_x,accel_y,accel_z;
	uint8_t buffer[16];
	stdio_init_all();
	sleep_ms(10000);
	printf("i2c setup");
#if !defined(i2c_default) || !defined(PICO_DEFAULT_I2C_SDA_PIN) || !defined(PICO_DEFAULT_I2C_SCL_PIN)
#warning i2c/bus_scan example requires a board with I2C pins
    puts("Default I2C pins were not defined");
#else
    // This example will use I2C0 on the default SDA and SCL pins (GP4, GP5 on a Pico)
    i2c_init(i2c_default, 400 * 1000);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
    // Make the I2C pins available to picotool
    bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));

	//Necessary, Wake up
    printf("\nI2C Bus Scan\n");
	buffer[0]=PWR_MGMT0;
	buffer[1]=0x00;
	retVal=i2c_write_blocking(i2c_default,MPU_ADDR,buffer,2,false);
	if(retVal<0){
		printf("\nUnable to set Reg");
	}
	else{
		printf("\n Set Register Power Mgt");
		retVal=i2c_read_blocking(i2c_default,MPU_ADDR,buffer,1,false);
		if(retVal<0){
			printf("\nUnable to Read!");
		}
		else{
			printf("\nPower %02X ,%02X",buffer[0],buffer[1]);
		}
	}
	printf("\nCollection begins");
    
	//Just a test. who am I?
	printf("\nI2C Bus Scan\n");
	buffer[0]=WHO_AM_I_REG;
	retVal=i2c_write_blocking(i2c_default,MPU_ADDR,buffer,1,false);
	if(retVal<0){
		printf("\nUnable to set Reg");
	}
	else{
		printf("\n Set Register Who am I");
		retVal=i2c_read_blocking(i2c_default,MPU_ADDR,buffer,1,false);
		if(retVal<0){
			printf("\nUnable to Read!");
		}
		else{
			printf("\nI am %02X ",buffer[0]);
		}
	}
	
	
	//Read AFS
    printf("\nI2C Bus Scan\n");
	buffer[0]=AFS;
	buffer[1]=0x00;//No test, set 2G
	retVal=i2c_write_blocking(i2c_default,MPU_ADDR,buffer,1,false);
	if(retVal<0){
		printf("\nUnable to set Reg");
	}
	else{
		printf("\n Set Register AFS");
		retVal=i2c_read_blocking(i2c_default,MPU_ADDR,buffer,1,false);
		if(retVal<0){
			printf("\nUnable to Read!");
		}
		else{
			printf("\nAFS %02X ",buffer[0]);
		}
	}
	
	//Read Accel 3 axis
	while(1){
	sleep_ms(1000);
    printf("\nI2C Bus Scan\n");
	buffer[0]=ACCEL;
	retVal=i2c_write_blocking(i2c_default,MPU_ADDR,buffer,1,false);
	if(retVal<0){
		printf("\nUnable to set Reg");
	}
	else{
		printf("\n Set Register Accel");
		retVal=i2c_read_blocking(i2c_default,MPU_ADDR,buffer,6,false);
		if(retVal<0){
			printf("\nUnable to Read!");
		}
		else{
			buffer[6]=buffer[4];
			buffer[4]=buffer[2];
			buffer[2]=buffer[0];
			accel_x=*((signed short*)&buffer[1]);
			accel_y=*((signed short*)&buffer[3]);
			accel_z=*((signed short*)&buffer[5]);
			printf("\nACCEL %hd,%hd,%hd,",accel_x,accel_y,accel_z);
		}
	}
	}
	
	//Read FS
    printf("\nI2C Bus Scan\n");
	buffer[0]=FS;
	buffer[1]=0x00;//No test, set 2G
	retVal=i2c_write_blocking(i2c_default,MPU_ADDR,buffer,1,false);
	if(retVal<0){
		printf("\nUnable to set Reg");
	}
	else{
		printf("\n Set Register FS");
		retVal=i2c_read_blocking(i2c_default,MPU_ADDR,buffer,1,false);
		if(retVal<0){
			printf("\nUnable to Read!");
		}
		else{
			printf("\nFS %02X ",buffer[0]);
		}
	}

	//Read Gyro 3 axis
    printf("\nI2C Bus Scan\n");
	buffer[0]=GYRO;
	retVal=i2c_write_blocking(i2c_default,MPU_ADDR,buffer,1,false);
	if(retVal<0){
		printf("\nUnable to set Reg");
	}
	else{
		printf("\n Set Register Gyro");
		retVal=i2c_read_blocking(i2c_default,MPU_ADDR,buffer,6,false);
		if(retVal<0){
			printf("\nUnable to Read!");
		}
		else{
			buffer[6]=buffer[4];
			buffer[4]=buffer[2];
			buffer[2]=buffer[0];
			gyro_x=*((signed short*)&buffer[1]);
			gyro_y=*((signed short*)&buffer[3]);
			gyro_z=*((signed short*)&buffer[5]);
			printf("\nGYRO %hd,%hd,%hd,",gyro_x,gyro_y,gyro_z);
		}
	}

	//Temperature
    printf("\nI2C Bus Scan\n");
	buffer[0]=TEMP_MSB;
	retVal=i2c_write_blocking(i2c_default,MPU_ADDR,buffer,1,false);
	if(retVal<0){
		printf("\nUnable to set Reg");
	}
	else{
		printf("\n Set Register Temp");
		retVal=i2c_read_blocking(i2c_default,MPU_ADDR,buffer,2,false);
		buffer[2]=buffer[0];
	    s=*((signed short*)&buffer[1]);
		printf("%hd",s);
		temp=((float)s/340.0)+36.53;
		if(retVal<0){
			printf("\nUnable to Read!");
		}
		else{
			printf("\nTEMP %f",temp);
		}
	}
	

#endif
	return 0;
}
