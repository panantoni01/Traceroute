#ifndef COMMON_H
#define COMMON_H

#include<stdio.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<sys/time.h>

#define ERR_EXIT(msg) \
    do { \
        perror(msg); \
        exit(EXIT_FAILURE); \
    } while(0)

typedef struct receive {
    int rec_icmp_type;
    struct in_addr rec_addr;
    struct timeval rec_send_time;
    struct timeval rec_rec_time;
} receive_t;

typedef enum {
    MODE_UDP,
    MODE_ICMP
} trace_mode_t;

typedef struct config {
    struct sockaddr_in address;
    struct timeval wait_time;
    int first_ttl;
    int max_ttl;
    int num_send;
    int use_dns; 
    trace_mode_t mode;
} config_t;

#endif /* COMMON_H */
