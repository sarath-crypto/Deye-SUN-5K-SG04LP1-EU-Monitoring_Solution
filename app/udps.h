#ifndef UDPS_H
#define UDPS_H

#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>
#include <errno.h>
#include <getopt.h>
#include <inttypes.h>
#include <ctype.h>
#include <byteswap.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <math.h>
#include <boost/thread.hpp>
#include <condition_variable>
#include <netdb.h>
#include <time.h>
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <vector>
#include "global.h"

using namespace std;

class udps{
        private:
                int sockfd;
                int len;
                struct sockaddr_in seraddr;
		struct sockaddr_in cliaddr;
                struct timeval timeout;
		string mip;
		string pip;
        public:
                vector <pdu>rxfifo;
                vector <pdu>txfifo_m;
                vector <pdu>txfifo_p;
                bool state;
                bool con;
                udps(string,string);
                ~udps();
                void recv(void);
		void send(void);
};

#endif

