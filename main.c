#include<stdio.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<netinet/ip_icmp.h>
#include<unistd.h>
#include<errno.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
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

    /* create sockadrr_in address structure */
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    Inet_pton(AF_INET, argv[1], &address.sin_addr);
    
    /* for each ttl value send and receive 3 icmp packs;
    if ECHOREPLY was received, receive_icmp() returns 1 */
    int ttl = 1;
    while (ttl <= 30) {
        send_icmp(sockfd, &address, &ttl, 3);
        if (receive_icmp(sockfd, &ttl, 3))
            break;
        ttl++;
    }

    return 0;
}
