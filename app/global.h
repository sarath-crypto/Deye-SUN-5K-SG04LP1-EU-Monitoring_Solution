#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <string>
#include <iostream>
#include <vector>

#define FRAME_SZ        0x8000
#define DAY_SEC         86400
#define HR_SEC          3600
#define CONFIG_DIR_PATH_SZ	1024
#define RSSI_MIN	-70
#define FRAME_W		1920
#define FRAME_H		1080
#define IRIMG_SZ	64
#define BUF_SZ  	512

using namespace std;

typedef struct pdu{
        unsigned char data[BUF_SZ];
        unsigned short len;
}pdu;

typedef struct frames{
        bool wr;
        unsigned char img_data[FRAME_SZ];
        unsigned short img_len;
	unsigned int ts;
}frames;

typedef struct iframes{
        bool wr;
	short therm;
	float ir_data[FRAME_SZ];
	float gr_data[FRAME_SZ];
	unsigned short ir_len;
	unsigned short gr_len;
}iframes;

typedef struct rtd{
	unsigned short pr;
	unsigned short al;
	unsigned short aqi;
	unsigned char bat;
 	unsigned char tempa;
	unsigned char tempb;
	bool rdr;
	unsigned short wl;
	unsigned short dprod;
	unsigned short dload;
	unsigned short dbuy;
	unsigned char soc;
	unsigned short uload;
	unsigned short gload;
	unsigned short prod;
	unsigned short gvolt;
	unsigned short gdexp;
	unsigned short gexp;
}rtd;

typedef struct uptme{
        unsigned long uts;
        unsigned short d;
        unsigned char  h;
        unsigned char  m;
}uptme;

typedef struct DEVICE_INFO{
        string device_description;
        string bus_info;
        vector<string> device_paths;
}DEVICE_INFO;

enum file_type{IMG = 1,IRI,TMR,LOG};

bool execute(string &);
void gettimestamp(string &,unsigned char);
void getuptime(uptme *);
void devlist(vector<DEVICE_INFO> &);
DEVICE_INFO resolve_path(const string &);

#endif
