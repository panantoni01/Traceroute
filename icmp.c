#include<stdint.h>
#include<stdio.h>
#include<assert.h>
#include<stdlib.h>
#include<netinet/ip.h>
#include<netinet/ip_icmp.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<errno.h>
#include<string.h>
#include<sys/time.h>
#include<netdb.h>

#include"icmp.h"
#include"wrappers.h"


static uint16_t compute_icmp_checksum (const void *buff, int length){
	uint32_t sum;
	const uint16_t* ptr = buff;
	assert (length % 2 == 0);
	for (sum = 0; length > 0; length -= 2)
		sum += *ptr++;
	sum = (sum >> 16) + (sum & 0xffff);
	return (uint16_t)(~(sum + (sum >> 16)));
}

/* send icmp echo request to a specific ADDRESS with given TLL and SEQ */
void send_icmp_echo(int sockfd, struct sockaddr_in* address, int ttl, int seq) {
    struct icmp header = {0};
    header.icmp_type = ICMP_ECHO;
    header.icmp_hun.ih_idseq.icd_id = htons(getpid());
    header.icmp_hun.ih_idseq.icd_seq = htons(seq);
    header.icmp_cksum = compute_icmp_checksum(&header, sizeof(header));

    Setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(int));
    Sendto(sockfd, &header, sizeof(header), 0, 
        (struct sockaddr*) address, sizeof(*address));
}

// ===========================================================

static inline struct icmp* get_icmp_pointer(struct ip* ip_header) {
    return (struct icmp*)((uint8_t*)ip_header + 4*ip_header->ip_hl);
}

static inline int check_id_seq(struct icmp* packet, int min_seq, int max_seq, int id) {
    if (ntohs(packet->icmp_hun.ih_idseq.icd_id) != id)
        return 0;
    if (ntohs(packet->icmp_hun.ih_idseq.icd_seq) < min_seq || 
        ntohs(packet->icmp_hun.ih_idseq.icd_seq) > max_seq)
        return 0;
    return 1;
}

/* If icmp package stored in `buffer` is destined for this instance
of traceroute return 1, else return 0 */
static int verify_icmp_pack(uint8_t* buffer, int min_seq, int max_seq, int id) {
    struct icmp* icmp_packet = get_icmp_pointer((struct ip*)buffer);

    if (icmp_packet->icmp_type == ICMP_TIME_EXCEEDED) {
        struct icmp* inner_icmp_packet = get_icmp_pointer(&(icmp_packet->icmp_dun.id_ip.idi_ip));
        if (!check_id_seq(inner_icmp_packet, min_seq, max_seq, id))
            return 0;
    }
    else if (icmp_packet->icmp_type == ICMP_ECHOREPLY) {
        if (!check_id_seq(icmp_packet, min_seq, max_seq, id))
            return 0;
    }
    else
        return 0;
    
    return 1;
}

static inline int get_icmp_type(uint8_t* buffer) {
    struct icmp* icmp_packet = get_icmp_pointer((struct ip*)buffer);
    return icmp_packet->icmp_type;
}


/* receive a single ICMP_TIME_EXCEEDED or ICMP_ECHOREPLY packet 
with seq value in range <min_seq, max_seq> */
void receive_icmp(int sockfd, int min_seq, int max_seq, receive_t* response) {
    fd_set descriptors;
    struct timeval timeout = { .tv_sec = 1, .tv_usec = 0 };
    ssize_t res;
    uint8_t buffer[IP_MAXPACKET];
    struct sockaddr_in sender;
    socklen_t sender_len = sizeof(sender);
    int icmp_type;

    do {
        FD_ZERO(&descriptors);
        FD_SET(sockfd, &descriptors);

        /* NOTE: "On Linux, select() modifies timeout to reflect the amount
        of time not slept; most other implementations don't do this" */
        res = select(sockfd + 1, &descriptors, NULL, NULL, &timeout);
        if (res < 0)
            perror("select");
        else if (res == 0) {
            response->rec_status = STATUS_TIMEOUT;
            return;
        }

        gettimeofday(&response->rec_time, NULL);

        res = recvfrom (sockfd, buffer, IP_MAXPACKET, MSG_DONTWAIT,
                (struct sockaddr*)&sender, &sender_len);
        if (res < 0)
            perror("recvfrom");
    }  
    while (!verify_icmp_pack(buffer, min_seq, max_seq, getpid()));

    icmp_type = get_icmp_type(buffer);
    if (icmp_type == ICMP_TIME_EXCEEDED)
        response->rec_status = STATUS_TTL_EXCEEDED;
    else if (icmp_type == ICMP_ECHOREPLY)
        response->rec_status = STATUS_ECHOREPLY;

    response->rec_addr = sender.sin_addr;
}

void print_report(int ttl, struct timeval* send_time, receive_t* responses, int num_packs) {
    int i, j, num_addrs = 0;
    struct in_addr distinct_addrs[num_packs];
    char ip_addr_buf[INET_ADDRSTRLEN];
    long elapsed_us = 0;
    struct timeval tv;

    if (responses[0].rec_status == STATUS_TIMEOUT) {
        printf("%d. *\n", ttl);
        return;
    }

    /* find distinct IP addresses */
    for (i = 0; i < num_packs; i++) {
        if (responses[i].rec_status != STATUS_ECHOREPLY &&
            responses[i].rec_status != STATUS_TTL_EXCEEDED)
            break;
        
        for (j = 0; j < num_addrs; j++) {
            if (responses[i].rec_addr.s_addr == distinct_addrs[j].s_addr)
                break;
        }
        if (j == num_addrs)
            distinct_addrs[num_addrs++] = responses[i].rec_addr;
    }

    printf("%d. ", ttl);
    for (i = 0; i < num_addrs; i++) {
        memset(ip_addr_buf, 0, sizeof(ip_addr_buf));
        Inet_ntop(AF_INET, &(distinct_addrs[i]), ip_addr_buf, INET_ADDRSTRLEN);
        printf("%s ", ip_addr_buf);
    }


    for (i = 0; i < num_packs; i++) {
        if (responses[i].rec_status != STATUS_ECHOREPLY &&
            responses[i].rec_status != STATUS_TTL_EXCEEDED) {
            printf(" ???\n");
            return; 
        }

        timersub(&responses[i].rec_time, send_time, &tv);
        elapsed_us += tv.tv_usec + 1e6*tv.tv_sec;
    } 
    printf(" %.3fms\n", (double)elapsed_us / 1000 / num_packs);
}

int destination_reached(receive_t* responses, int num_packs) {
    for (int i = 0; i < num_packs; i++)
        if (responses[i].rec_status != STATUS_ECHOREPLY)
            return 0;
    return 1;
}