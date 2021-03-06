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
    int first_ttl = 1, max_ttl = 30;
    int use_dns = 1;
    int n = 3;

    int opt;
    while ((opt = getopt(argc, argv, "f:m:nq:")) != -1) {
        switch (opt) {
            case 'n':
                use_dns = 0;
                break;
            case 'f':
                first_ttl = atoi(optarg);
                break;
            case 'm':
                max_ttl = atoi(optarg);
                break;
            case 'q':
                n = atoi(optarg);
                if (n <= 0) {
                    fprintf(stderr, "Invalid argument for option -q\n");
                    exit(EXIT_FAILURE);
                }
                break;
            default:
                fprintf(stderr, "Usage: %s [-f first_ttl] [-m max_ttl] [-n] [-q num_packs] ip_addr\n",
                           argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "Expected argument after options\n");
        exit(EXIT_FAILURE);
    }
    
    /* create raw socket */
    int sockfd = Socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

    /* create sockadrr_in address structure */
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    Inet_pton(AF_INET, argv[optind], &address.sin_addr);
    
    /* for each ttl value send and receive `n` icmp packs */
    for (int ttl = first_ttl; ttl <= max_ttl; ttl++) {
        send_icmp(sockfd, &address, ttl, n);
        if (receive_icmp(sockfd, ttl, n, use_dns))
            break;
    }

    return 0;
}
