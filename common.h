#ifndef COMMON_H
#define COMMON_H

#include<stdio.h>
#include<stdlib.h>

#define ERR_EXIT(msg) \
    do { \
        perror(msg); \
        exit(1); \
    } while(0)

typedef struct receive {
    int rec_icmp_type;
    struct in_addr rec_addr;
    struct timeval rec_send_time;
    struct timeval rec_rec_time;
} receive_t;

#endif /* COMMON_H */
