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


// example.org: 93.184.216.34
int main (int argc, char* argv[]) {
    int first_ttl = 1, max_ttl = 30, ttl, i, num_packs = 3, opt, sockfd;
    struct sockaddr_in address;
    static int seq = 0;
    char* report;
    struct timeval tv;
    
    while ((opt = getopt(argc, argv, "f:m:q:")) != -1) {
        switch (opt) {
            case 'f':
                first_ttl = atoi(optarg);
                break;
            case 'm':
                max_ttl = atoi(optarg);
                break;
            case 'q':
                num_packs = atoi(optarg);
                if (num_packs <= 0) {
                    fprintf(stderr, "Invalid argument for option -q\n");
                    exit(EXIT_FAILURE);
                }
                break;
            default:
                fprintf(stderr, "Usage: %s [-f first_ttl] [-m max_ttl] [-q num_packs] ip_addr\n",
                           argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "Expected argument after options\n");
        exit(EXIT_FAILURE);
    }
    
    /* create raw socket */
    sockfd = Socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

    /* create sockadrr_in address structure */
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    Inet_pton(AF_INET, argv[optind], &address.sin_addr);
    
    for (ttl = first_ttl; ttl <= max_ttl; ttl++) {
        /* get current timestamp to measure time between sending echo requests
        and receiving a response ICMP_TIME_EXCEEDED or ICMP_ECHOREPLY */
        gettimeofday(&tv, NULL);
        
        /* send echo requests with given ttl and seq values */
        for (i = 0; i < num_packs; i++)
            send_icmp_echo(sockfd, &address, ttl, seq++);
        
        receive_t responses[num_packs];
        memset(responses, 0, sizeof(receive_t) * num_packs);
        for (i = 0; i < num_packs; i++) {
            receive_icmp(sockfd, seq - num_packs, seq - 1, &responses[i]);
            if (responses[i].rec_status == STATUS_TIMEOUT)
                break;
        }

        report = get_report(responses, num_packs);
        printf("%d. %s\n", ttl, report);
        free(report);

        if (destination_reached(responses, num_packs))
            break;
    }

    return 0;
}
