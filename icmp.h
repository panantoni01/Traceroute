#ifndef ICMP_H
#define ICMP_H

#include<stdint.h>
#include<stdio.h>
#include<assert.h>
#include<stdlib.h>
#include<netinet/ip.h>
#include<netinet/ip_icmp.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<errno.h>
#include<string.h>


void send_icmp (int sockfd, struct sockaddr_in* address, int* ttl, int seq);
int receive_icmp(int sockfd, int* ttl, int n);

#endif /* !ICMP_H */