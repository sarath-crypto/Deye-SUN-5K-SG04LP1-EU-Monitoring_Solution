#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <sys/ioctl.h>
#include <math.h>
#include "bmp180.h"

bmp180::bmp180(){
	status = true;
	if ((fd = open(I2CBus,O_RDWR)) < 0)status = false;
}

bmp180::~bmp180(){
}

void bmp180::readint(uint8_t *devValues,uint8_t startReg,uint8_t bytesToRead){
	struct i2c_rdwr_ioctl_data messagebuffer;
	struct i2c_msg read_reg[2]={
		{BMPx8x_I2CADDR,0,1,&startReg},
		{BMPx8x_I2CADDR,I2C_M_RD,bytesToRead,devValues}
	};
	messagebuffer.nmsgs = 2;                 
	messagebuffer.msgs = read_reg;
	ioctl(fd, I2C_RDWR, &messagebuffer); 
}

void  bmp180::writecmd(uint8_t devAction){
	struct i2c_rdwr_ioctl_data messagebuffer;
	uint8_t datatosend[2];
	datatosend[0]=BMPx8x_CtrlMeas;
	datatosend[1]=devAction;
	struct i2c_msg write_reg[1]={
		{BMPx8x_I2CADDR,0,2,datatosend}
	};
	messagebuffer.nmsgs = 1;  
	messagebuffer.msgs = write_reg;  
	ioctl(fd, I2C_RDWR, &messagebuffer); 
}

void bmp180::calibration(void){
  	uint8_t rValue[21];
  	readint(rValue,0xAA,22);
	bmp_ac1=((rValue[0]<<8)|rValue[1]);
	bmp_ac2=((rValue[2]<<8)|rValue[3]);
	bmp_ac3=((rValue[4]<<8)|rValue[5]);
	bmp_ac4=((rValue[6]<<8)|rValue[7]);
	bmp_ac5=((rValue[8]<<8)|rValue[9]);
	bmp_ac6=((rValue[10]<<8)|rValue[11]);
	bmp_b1=((rValue[12]<<8)|rValue[13]);
	bmp_b2=((rValue[14]<<8)|rValue[15]);
	bmp_mb=((rValue[16]<<8)|rValue[17]);
	bmp_mc=((rValue[18]<<8)|rValue[19]);
	bmp_md=((rValue[20]<<8)|rValue[21]);
}

void bmp180::waitforconversion(void){
	uint8_t rValues[3];
	int counter=0;
	do{
		sleepms (BMPx8x_RetryDelay);
		readint(rValues,BMPx8x_CtrlMeas,1);
		counter++;
	}while ( ((rValues[0] & 0x20) != 0)  &&  counter < 20 );  
}

void bmp180::getpressure(double *Pres){
	unsigned int up;  
	uint8_t rValues[3];
	writecmd((BMPx8x_PresConversion0+(BMPx8x_OverSampling<<6)));
	sleepms (BMPx8x_minDelay);
	waitforconversion();
	readint(rValues, BMPx8x_Results,3);
	up = (((unsigned int) rValues[0] << 16) | ((unsigned int) rValues[1] << 8) | (unsigned int) rValues[2]) >> (8-BMPx8x_OverSampling);
	int x1, x2, x3, b3, b6, p;
	unsigned int b4, b7;
	b6 = bmp_b5 - 4000;
	x1 = (bmp_b2 * (b6 * b6)>>12)>>11;
	x2 = (bmp_ac2 * b6)>>11;
	x3 = x1 + x2;
	b3 = (((((int)bmp_ac1)*4 + x3)<<BMPx8x_OverSampling) + 2)>>2;
	x1 = (bmp_ac3 * b6)>>13;
	x2 = (bmp_b1 * ((b6 * b6)>>12))>>16;
	x3 = ((x1 + x2) + 2)>>2;
	b4 = (bmp_ac4 * (unsigned int)(x3 + 32768))>>15;
	b7 = ((unsigned int)(up - b3) * (50000>>BMPx8x_OverSampling));
	if (b7 < 0x80000000)
		p = (b7<<1)/b4;
	else
		p = (b7/b4)<<1;

	x1 = (p>>8) * (p>>8);
	x1 = (x1 * 3038)>>16;
	x2 = (-7357 * p)>>16;
	p += (x1 + x2 + 3791)>>4;
	*Pres = ((double)p/(float)100.0);  
}

void bmp180::gettemperature(double *Temp){
	unsigned int ut;
	uint8_t rValues[2];
	writecmd(BMPx8x_TempConversion);
	sleepms (BMPx8x_minDelay);
	waitforconversion();
	readint(rValues, BMPx8x_Results,2);
	ut=((rValues[0]<<8)|rValues[1]);  

	int x1, x2;
	x1 = (((int)ut - (int)bmp_ac6)*(int)bmp_ac5) >> 15;
	x2 = ((int)bmp_mc << 11)/(x1 + bmp_md);
	bmp_b5 = x1 + x2;

	double result = ((bmp_b5 + 8)>>4);  
	*Temp = result/10;
}

double bmp180::altitude(double p){
  	return 145437.86*(1- pow((p/1013.25),0.190294496)); 
}
