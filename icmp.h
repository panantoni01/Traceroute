#ifndef ICMP_H
#define ICMP_H

#include <arpa/inet.h>
#include <sys/time.h>


typedef struct receive {
    int rec_icmp_type;
    struct in_addr rec_addr;
    struct timeval rec_send_time;
    struct timeval rec_rec_time;
} receive_t;


void send_icmp_echo(int sockfd, struct sockaddr_in* address, int ttl, int seq);
int receive_icmp(int sockfd, int min_seq, int max_seq, struct timeval* wait_time, receive_t* response);
int destination_reached(receive_t* responses, int num_send, int num_recv);

#endif /* !ICMP_H */
