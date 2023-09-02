#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <netdb.h>

#include "icmp.h"
#include "udp.h"
#include "common.h"

config_t config = {.wait_time = {.tv_sec = 1, .tv_usec = 0},
                   .first_ttl = 1,
                   .max_ttl = 30,
                   .num_send = 3,
                   .use_dns = 1,
                   .mode = MODE_ICMP,
                   .dest_port = 33434};

static void hostname_to_address(const char *hostname, struct in_addr *address) {
    struct addrinfo hint;
    struct addrinfo *result;

    memset(&hint, 0, sizeof(hint));
    hint.ai_family = AF_INET;

    if (getaddrinfo(hostname, NULL, &hint, &result) != 0)
        eprintf("getaddrinfo:");
    *address = ((struct sockaddr_in *)result->ai_addr)->sin_addr;
    freeaddrinfo(result);
}

int main(int argc, char *argv[]) {
    int opt;

    while ((opt = getopt(argc, argv, "IUnf:m:q:w:s:")) != -1) {
        switch (opt) {
            case 'n':
                config.use_dns = 0;
                break;
            case 'f':
                config.first_ttl = atoi(optarg);
                break;
            case 'm':
                config.max_ttl = atoi(optarg);
                break;
            case 'q':
                config.num_send = atoi(optarg);
                if (config.num_send <= 0) {
                    fprintf(stderr, "Invalid argument for option -q\n");
                    exit(EXIT_FAILURE);
                }
                break;
            case 'w':
                config.wait_time.tv_sec = (long int)atoi(optarg);
                break;
            case 's':
                config.dest_port = (uint16_t)atoi(optarg);
                break;
            case 'U':
                config.mode = MODE_UDP;
                break;
            case 'I':
                config.mode = MODE_ICMP;
                break;
            default:
                fprintf(
                  stderr,
                  "Usage: %s [-f first_ttl] [-m max_ttl] [-q num_send] [-w wait_time] ip_addr\n",
                  argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "Expected argument after options\n");
        exit(EXIT_FAILURE);
    }

    memset(&config.address, 0, sizeof(config.address));
    config.address.sin_family = AF_INET;
    hostname_to_address(argv[optind], &config.address.sin_addr);

    if (config.mode == MODE_ICMP)
        icmp_main();
    else if (config.mode == MODE_UDP)
        udp_main();
    else {
        fprintf(stderr, "Unknown traceroute mode: %u\n", config.mode);
        exit(EXIT_FAILURE);
    }

    return 0;
}
