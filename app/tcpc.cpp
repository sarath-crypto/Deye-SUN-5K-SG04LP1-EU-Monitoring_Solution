#include <math.h>
#include <fstream>

#include "tcpc.h"

#define PORT	8899
extern bool ex;

rxcom::~rxcom(){
	close(sockfd);
}
rxcom::rxcom(string ip){
	state = true;
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ){
        	state = false;
		return;
	}
	bzero(&seraddr,sizeof(seraddr));
	seraddr.sin_family  = AF_INET;
	inet_pton(AF_INET, ip.c_str(), &(seraddr.sin_addr));
	seraddr.sin_port = htons(PORT);
	timeout.tv_sec = 0;
	timeout.tv_usec = 10;
	setsockopt(sockfd,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout));
	con = false;
}

void rxcom::send(void){
        if(con && txfifo.size()){
                pdu  p = txfifo[0];
                int n = -1;
                n = write(sockfd,(void *)&p.data,p.len);
                if(n == p.len)txfifo.erase(txfifo.begin());
        }
}

void rxcom::recv(void){
	if(state && !con){
		if (connect(sockfd, (const struct sockaddr *)&seraddr, sizeof(seraddr)) != 0) { 
        		printf("Server failed\n"); 
			sleep(1);
			return;
    		}
		con = true;
	}else{	
		pdu p;
		int n = read(sockfd,(void *)&p.data,sizeof(p.data)); 
       		if(n > 0){
			p.len = n;
			rxfifo.push_back(p);
		}
	}
}
