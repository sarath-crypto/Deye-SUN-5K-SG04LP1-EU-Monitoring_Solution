#include <wiringPi.h>
#include "amg8833.h"

amg8833::amg8833(){
	status =  false;
	for (uint8_t i=0; i<128; i++){
		rawData[i]=0;
	}
	fd = wiringPiI2CSetup(AMG8833_ADDRESS);
	if (fd == -1) {
		std::cout << "Failed to init I2C communication.\n";
		return;
	}
	status = true;
}

void amg8833::init(uint8_t mode, uint8_t fps){
	wiringPiI2CWriteReg8(fd, AMG8833_RST, AMG8833_SOFTWARE_RESET);
	wiringPiI2CWriteReg8(fd, AMG8833_PCTL, mode);
	wiringPiI2CWriteReg8(fd, AMG8833_FPSC, fps);
	wiringPiI2CWriteReg8(fd, AMG8833_AVE,0x20);
}

void  amg8833::readBytes(uint8_t registerAddress, uint8_t count, uint8_t * dest) {
	for(unsigned char i = 0;i < count;i++){
		*(dest+i) = readByte(registerAddress+i);
	}
}

uint8_t amg8833::readByte(uint8_t registerAddress) {
	uint8_t value = wiringPiI2CReadReg16(fd, registerAddress);
	return value;
}

void amg8833::writeByte(uint8_t registerAddress, uint8_t data){
	wiringPiI2CWriteReg8(fd, registerAddress,data);
}

int16_t amg8833::readThermistor(){
	uint8_t rawData[2] = {0, 0};
	readBytes(AMG8833_TTHL, 2, &rawData[0]);
	return (int16_t) (((int16_t) rawData[1] << 8) | rawData[0]);
}

void amg8833::readGrid(float* tempValues){
	readBytes(AMG8833_DATA01L, 128, &rawData[0]);
	for(uint16_t ii = 0; ii < 64; ii++) {
		tempValues[ii] = (float) ((int16_t) ( (int16_t) rawData[2*ii + 1] << 8) | rawData[2*ii]);
		tempValues[ii] *=0.25f; 
	}
}
