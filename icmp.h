#ifndef ICMP_H
#define ICMP_H

#include <arpa/inet.h>
#include <sys/time.h>


typedef struct receive {
    int rec_icmp_type;
    struct in_addr rec_addr;
    struct timeval rec_time;
} receive_t;


void send_icmp_echo(int sockfd, struct sockaddr_in* address, int ttl, int seq);
int receive_icmp(int sockfd, int min_seq, int max_seq, receive_t* response);
void print_report(int ttl, struct timeval* send_time, receive_t* responses, int num_send, int num_recv);
int destination_reached(receive_t* responses, int num_send, int num_recv);

#endif /* !ICMP_H */