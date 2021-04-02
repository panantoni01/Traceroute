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


uint16_t compute_icmp_checksum (const void* buff, int length);
void set_icmp_header(struct icmp* header, int seq);
void send_icmp (int sockfd, struct sockaddr_in* address, int* ttl, int seq);
void receive_icmp(int sockfd, struct sockaddr_in* address);

#endif /* !ICMP_H */