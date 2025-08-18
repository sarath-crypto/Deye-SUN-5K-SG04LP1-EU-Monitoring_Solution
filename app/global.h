#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <string>
#include <iostream>

#define FRAME_SZ        0x8000
#define DAY_SEC         86400
#define HR_SEC          3600
#define CONFIG_DIR_PATH_SZ	1024
#define RSSI_MIN	-70
#define FRAME_W		1920
#define FRAME_H		1080
#define IRIMG_SZ	64

using namespace std;

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
	unsigned char  aqi;
	unsigned char bat;
 	unsigned char tempa;
	unsigned char tempb;
	bool rdr;
	unsigned short dprod;
       	unsigned short l1;
	unsigned short l2;
	unsigned char soc;
	unsigned short pv1;
	unsigned short pv2;
	unsigned char grid;
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
