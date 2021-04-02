#include<stdio.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<netinet/ip_icmp.h>
#include<unistd.h>
#include<errno.h>
#include<string.h>
#include<stdlib.h>
#include"icmp.h"

// example.org: 93.184.216.34
int main (int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s <ip_addr>\n", argv[0]);
        exit(1);
    }

     /* create raw socket */
    int sockfd; 
    
    if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) {
        fprintf(stderr, "socket error: %s\n", strerror(errno));
        exit(1);
    }

    /* create sockarr_in address structure */
    struct sockaddr_in address;
    int n;
    
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    if ((n = inet_pton(AF_INET, argv[1], &address.sin_addr)) <= 0) {
        if (n == 0)
            fprintf(stderr, "ERROR: invalid ip address!\n");
        else 
            fprintf(stderr, "inet_pton error: %s\n", strerror(errno));
        exit(1);
    }

    int ttl = 64;
    /* send ping request and receive reply */
    send_icmp(sockfd, &address, &ttl, 1);
    receive_icmp(sockfd, &address);

    return 0;
}
