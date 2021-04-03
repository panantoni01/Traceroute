#include<stdio.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<netinet/ip_icmp.h>
#include<unistd.h>
#include<errno.h>
#include<string.h>
#include<stdlib.h>
#include"icmp.h"
#include"wrappers.h"

// example.org: 93.184.216.34
int main (int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s <ip_addr>\n", argv[0]);
        exit(1);
    }

     /* create raw socket */
    int sockfd;
    sockfd = Socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

    /* create sockarr_in address structure */
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    Inet_pton(AF_INET, argv[1], &address.sin_addr);
    
    int ttl = 64;
    /* send ping request and receive reply */
    send_icmp(sockfd, &address, &ttl, 1);
    
    struct sockaddr_in sender;
    receive_icmp(sockfd, &sender);

    return 0;
}
