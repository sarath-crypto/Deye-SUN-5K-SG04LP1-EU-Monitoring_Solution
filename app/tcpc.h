#ifndef _TCPC_H
#define _TCPC_H

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
#include <fstream>

using namespace std;

class tcpc{
	private:
		int sockfd;
		int len;
		struct sockaddr_in seraddr;
		struct timeval timeout;
	public:
		vector <pdu>rxfifo;
		vector <pdu>txfifo;
		bool state;
		bool con;
		tcpc(string);
		~tcpc();
		void recv(void);
		void send(void);
};

#endif
