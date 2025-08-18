#ifndef _BMP180_H_
#define _BMP180_H_

#define sleepms(ms)  usleep((ms)*1000)

#define BMPx8x_I2CADDR           0x77
#define BMPx8x_CtrlMeas          0xF4
#define BMPx8x_TempConversion    0x2E
#define BMPx8x_PresConversion0   0x34
#define BMPx8x_Results           0xF6
#define BMPx8x_minDelay          4     //require 4.5ms *1000/700 'turbo mode fix'= 6.4-Retry =4.4
#define BMPx8x_RetryDelay        2     //min delay for temp 4+2=6ms, max 4+2*20=44ms for pressure
                                       //Will stop waiting if conversion is complete

#define I2CBus             "/dev/i2c-1"      

class bmp180{
	private:
	int fd;
	const unsigned char BMPx8x_OverSampling = 3;
	short int            bmp_ac1;
	short int            bmp_ac2; 
	short int            bmp_ac3; 
	unsigned short int   bmp_ac4;
	unsigned short int   bmp_ac5;
	unsigned short int   bmp_ac6;
	short int            bmp_b1; 
	short int            bmp_b2;
	int                  bmp_b5; 
	short int            bmp_mb;
	short int            bmp_mc;
	short int            bmp_md;

	public:
	bool status;
	bmp180();
	~bmp180();
	void readint(uint8_t *devValues,uint8_t startReg,uint8_t bytesToRead);
	void writecmd(uint8_t devAction);
	void calibration(void);
	void waitforconversion(void);
	void getpressure(double *Pres);
	void gettemperature(double *Temp);
	double altitude(double p);
	double qnh(double p,double StationAlt);
	double ppl_DensityAlt(double PAlt,double Temp);
};
#endif
