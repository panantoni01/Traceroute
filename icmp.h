#ifndef ICMP_H
#define ICMP_H

#include <arpa/inet.h>
#include <sys/time.h>

typedef enum {
    STATUS_TTL_EXCEEDED = 1,
    STATUS_ECHOREPLY = 2,
    STATUS_TIMEOUT = 3
} icmp_status_t;

typedef struct receive {
    icmp_status_t rec_status;
    struct in_addr rec_addr;
    struct timeval rec_time;
} receive_t;


void send_icmp_echo(int sockfd, struct sockaddr_in* address, int ttl, int seq);
void receive_icmp(int sockfd, int min_seq, int max_seq, receive_t* response);
void print_report(int ttl, struct timeval* send_time, receive_t* responses, int num_packs);
int destination_reached(receive_t* responses, int num_packs);

#endif /* !ICMP_H */