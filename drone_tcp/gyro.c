#include "gyro.h"
#include "auxiliary_funcs.h"

//PLEASE ENABLE DLPF
//Please check for error


void i2c_begin(){
	int retVal;
	signed short s;
	uint8_t buffer[4];

	printf("i2c setup");

	// This example will use I2C0 on the default SDA and SCL pins (GP4, GP5 on a Pico)
    i2c_init(MPU_I2C, 400 * 1000);
    gpio_set_function(MPU_I2C_SCL_GPIO, GPIO_FUNC_I2C);
    gpio_set_function(MPU_I2C_SDA_GPIO, GPIO_FUNC_I2C);
    gpio_pull_up(MPU_I2C_SCL_GPIO);
    gpio_pull_up(MPU_I2C_SDA_GPIO);
    // Make the I2C pins available to picotool
    bi_decl(bi_2pins_with_func(MPU_I2C_SDA_PIN, MPU_I2C_SCL_PIN, GPIO_FUNC_I2C));

	//Necessary, Wake up
    printf("\nI2C Bus Scan\n");
	buffer[0]=PWR_MGMT0;
	buffer[1]=0x01;
	retVal=i2c_write_blocking(i2c_default,MPU_ADDR,buffer,2,false);
	if(retVal<0){
		printf("\nUnable to set Reg");
	}
	else{
		printf("\nSet Register Power Mgt");
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
		printf("\nSet Register Who am I");
		retVal=i2c_read_blocking(i2c_default,MPU_ADDR,buffer,1,false);
		if(retVal<0){
			printf("\nUnable to Read!");
		}
		else{
			printf("\nI am %02X ",buffer[0]);
		}
	}
	
	//Config
	printf("\nI2C Bus Scan\n");
	buffer[0]=CONFIG;
	buffer[1]=0x04;//Set
	retVal=i2c_write_blocking(i2c_default,MPU_ADDR,buffer,1,false);
	if(retVal<0){
		printf("\nUnable to set Reg");
	}
	else{
		printf("\nSet Register DLPF");
		retVal=i2c_read_blocking(i2c_default,MPU_ADDR,buffer,1,false);
		if(retVal<0){
			printf("\nUnable to Read!");
		}
		else{
			printf("\nDLPF %02X ",buffer[0]);
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
		printf("\nSet Register AFS");
		retVal=i2c_read_blocking(i2c_default,MPU_ADDR,buffer,1,false);
		if(retVal<0){
			printf("\nUnable to Read!");
		}
		else{
			printf("\nAFS %02X ",buffer[0]);
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
		printf("\nSet Register FS");
		retVal=i2c_read_blocking(i2c_default,MPU_ADDR,buffer,1,false);
		if(retVal<0){
			printf("\nUnable to Read!");
		}
		else{
			printf("\nFS %02X ",buffer[0]);
		}
	}
	//Temperature - optional
    printf("\nI2C Bus Scan\n");
	buffer[0]=TEMP_MSB;
	retVal=i2c_write_blocking(i2c_default,MPU_ADDR,buffer,1,false);
	if(retVal<0){
		printf("\nUnable to set Reg");
	}
	else{
		printf("\nSet Register Temp");
		retVal=i2c_read_blocking(i2c_default,MPU_ADDR,buffer,2,false);
		buffer[2]=buffer[0];
	    s=*((signed short*)&buffer[1]);
		float temp=((float)s/340.0)+36.53;
		if(retVal<0){
			printf("\nUnable to Read!");
		}
		else{
			printf("\nTEMP %f\n",temp);
		}
	}
}

bool timer_callback(__unused struct repeating_timer *t){
	returner temp;
	temp = i2c_accel();
	memcpy((void*)&mpu,(void*)&temp,sizeof(returner));
	temp = i2c_gyro();
	memcpy((void*)((char*)&mpu)+6,(void*)&temp,sizeof(returner));
	return true;
}

void timed_accel_gyro(){
	static struct repeating_timer timer;
	if(add_repeating_timer_ms(-20, timer_callback, NULL, &timer)){//Make this a define
		printf("Timer Added Successfully\n");
		return;
	}
	printf("Timer Could not be Added\n");
	return;
}

returner i2c_accel(){
	int retVal;
	uint8_t buffer[8];
	returner r;
	//Read Accel 3 axis
    //printf("\nI2C Bus Scan\n");
	buffer[0]=ACCEL;
	retVal=i2c_write_blocking(i2c_default,MPU_ADDR,buffer,1,false);
	if(retVal<0){
		printf("\nUnable to set Reg");
	}
	else{
		//printf("\n Set Register Accel");
		retVal=i2c_read_blocking(i2c_default,MPU_ADDR,buffer,6,false);
		if(retVal<0){
			printf("\nUnable to Read!");
		}
		else{
			buffer[6]=buffer[4];
			buffer[4]=buffer[2];
			buffer[2]=buffer[0];
			r.val1=*((signed short*)&buffer[1]);
			r.val2=*((signed short*)&buffer[3]);
			r.val3=*((signed short*)&buffer[5]);
			//printf("\nACCEL %hd,%hd,%hd,",r.val1,r.val2,r.val3);
		}
	}
	return r;
}

returner i2c_gyro(){
	int retVal;
	returner r;
	uint8_t buffer[8];
	//Read Gyro 3 axis
    //printf("\nI2C Bus Scan\n");
	buffer[0]=GYRO;
	retVal=i2c_write_blocking(i2c_default,MPU_ADDR,buffer,1,false);
	if(retVal<0){
		printf("\nUnable to set Reg");
	}
	else{
		//printf("\n Set Register Gyro");
		retVal=i2c_read_blocking(i2c_default,MPU_ADDR,buffer,6,false);
		if(retVal<0){
			printf("\nUnable to Read!");
		}
		else{
			buffer[6]=buffer[4];
			buffer[4]=buffer[2];
			buffer[2]=buffer[0];
			r.val1=*((signed short*)&buffer[1]);
			r.val2=*((signed short*)&buffer[3]);
			r.val3=*((signed short*)&buffer[5]);
			//printf("\nGYRO %hd,%hd,%hd,",r.val1,r.val2,r.val3);
		}
	}

	return r;
}

