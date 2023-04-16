#include<stdio.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<netinet/ip_icmp.h>
#include<unistd.h>
#include<errno.h>
#include<string.h>
#include<stdlib.h>
#include<sys/time.h>
#include<time.h>
#include"icmp.h"
#include"wrappers.h"


int main (int argc, char* argv[]) {
    int first_ttl = 1, max_ttl = 30, ttl, i, num_send = 3, num_recv, opt, sockfd;
    struct sockaddr_in address;
    static int seq = 0;
    struct timeval wait_time = { .tv_sec = 1, .tv_usec = 0 }, send_time;
    
    while ((opt = getopt(argc, argv, "f:m:q:w:")) != -1) {
        switch (opt) {
            case 'f':
                first_ttl = atoi(optarg);
                break;
            case 'm':
                max_ttl = atoi(optarg);
                break;
            case 'q':
                num_send = atoi(optarg);
                if (num_send <= 0) {
                    fprintf(stderr, "Invalid argument for option -q\n");
                    exit(EXIT_FAILURE);
                }
                break;
            case 'w':
                wait_time.tv_sec = (long int)atoi(optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s [-f first_ttl] [-m max_ttl] [-q num_send] [-w wait_time] ip_addr\n",
                           argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "Expected argument after options\n");
        exit(EXIT_FAILURE);
    }
    
    sockfd = Socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    Inet_pton(AF_INET, argv[optind], &address.sin_addr);
    
    for (ttl = first_ttl; ttl <= max_ttl; ttl++) {
        Gettimeofday(&send_time, NULL);

        for (i = 0; i < num_send; i++)
            send_icmp_echo(sockfd, &address, ttl, seq++);
        
        receive_t responses[num_send];
        memset(responses, 0, num_send * sizeof(receive_t));
        num_recv = 0;
        for (i = 0; i < num_send; i++) {
            if (receive_icmp(sockfd, seq - num_send, seq - 1, &wait_time, &responses[i]) == 0)
                break;
            num_recv++;
        }

        print_report(ttl, &send_time, responses, num_send, num_recv);

        if (destination_reached(responses, num_send, num_recv))
            break;
    }

    return 0;
}
