#include <fstream>
#include <iostream>
#include "global.h"
#include "udps.h"

#define PORT	8880

//#define DEBUG	1

using namespace std;

udps::~udps(){
	close(sockfd);
}

udps::udps(string m_ip,string p_ip){
	state = true;
	if((sockfd = socket(AF_INET,SOCK_DGRAM, 0)) < 0 ){
        	state = false;
		return;
	}
	bzero(&seraddr,sizeof(seraddr));
	bzero(&cliaddr,sizeof(cliaddr));
	seraddr.sin_family    = AF_INET;
	seraddr.sin_addr.s_addr = INADDR_ANY;
	seraddr.sin_port = htons(PORT);
	if(bind(sockfd,(const struct sockaddr *)&seraddr,sizeof(seraddr)) < 0){
        	state = false;
		return;
    	}
	timeout.tv_sec = 0;
	timeout.tv_usec = 10;
	setsockopt(sockfd,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout));
	len = sizeof(cliaddr);
	mip = m_ip;
	pip = p_ip;
}

void udps::recv(void){
	int n = -1;
	pdu p;
    	n = recvfrom(sockfd,(char *)&p.data,sizeof(p),MSG_WAITALL,( struct sockaddr *) &cliaddr,(socklen_t*)&len); 
	if(n > 0){
		p.len = n;
#ifdef DEBUG
		fprintf(stderr,"RECV->BYTES:%d\n",n);
#endif
		rxfifo.push_back(p);
	}
}

void udps::send(void){
	pdu p;
	if(txfifo_m.size()){
		cliaddr.sin_family = AF_INET;
		cliaddr.sin_port = htons(PORT);
		inet_aton(mip.c_str(),&cliaddr.sin_addr);
		pdu p = txfifo_m[0];
		int n = -1;
		n = sendto(sockfd,(const char *)&p.data,p.len,MSG_CONFIRM, (const struct sockaddr *) &cliaddr,sizeof(cliaddr));
		if(n == p.len){
#ifdef	DEBUG
			char buffer[1024];
			printf("%s M SEND->BYTES:%d Q:%d\n", inet_ntop(AF_INET,&cliaddr.sin_addr,(char *)&buffer,1024),n,txfifo_m.size());
#endif
			if(txfifo_m.size())txfifo_m.erase(txfifo_m.begin());
		}
	}
	if(txfifo_p.size()){
		cliaddr.sin_family = AF_INET;
		cliaddr.sin_port = htons(PORT);
		inet_aton(pip.c_str(),&cliaddr.sin_addr);
		pdu p = txfifo_p[0];
		int n = -1;
		n = sendto(sockfd,(const char *)&p.data,p.len,MSG_CONFIRM, (const struct sockaddr *) &cliaddr,sizeof(cliaddr));
		if(n == p.len){
#ifdef	DEBUG
			char buffer[1024];
			printf("%s P SEND->BYTES:%d Q:%d\n", inet_ntop(AF_INET,&cliaddr.sin_addr,(char *)&buffer,1024),n,txfifo_p.size());
#endif
			if(txfifo_p.size())txfifo_p.erase(txfifo_p.begin());
		}
	}

}
