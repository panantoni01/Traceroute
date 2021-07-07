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
#include<sys/time.h>
#include<netdb.h>

#define   NI_MAXHOST 1025

void send_icmp (int sockfd, struct sockaddr_in* address, int* ttl, int n);
int receive_icmp(int sockfd, int* ttl, int n, int map_IP_addr);

#endif /* !ICMP_H */