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
    ssize_t ret;
    struct icmp header = {0};
    header.icmp_type = ICMP_ECHO;
    header.icmp_hun.ih_idseq.icd_id = htons(getpid());
    header.icmp_hun.ih_idseq.icd_seq = htons(seq);
    header.icmp_cksum = compute_icmp_checksum(&header, sizeof(header));

    ret = setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(int));
    if (ret < 0)
        perror("setsockopt");
    
    ret = sendto(sockfd, &header, sizeof(header), 0, 
        (struct sockaddr*) address, sizeof(*address));
    if (ret < 0)
        perror("sendto");
}

// ================================================================

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

/* If icmp package stored in `buffer` is destined for this instance of traceroute, i.e:
** its type is ICMP_TIME_EXCEEDED or ICMP_ECHOREPLY and
** its id is equal to `id` and
** its seq value is from range <`min_seq`, `max_seq`>
then return 1, else 0 */
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


/* Receive a single icmp packet destined to this instance of 
tracerotute and store packet info (type, IP address, response time) in `response`.
Return 1 if a package, that is destined to this instance of 
tracerotute was received or 0 if `wait_time` exceeded. */
int receive_icmp(int sockfd, int min_seq, int max_seq, struct timeval* wait_time, receive_t* response) {
    ssize_t ret;
    fd_set descriptors;
    struct timeval timeout = *wait_time;
    uint8_t buffer[IP_MAXPACKET];
    struct sockaddr_in sender;
    socklen_t sender_len = sizeof(sender);

    do {
        FD_ZERO(&descriptors);
        FD_SET(sockfd, &descriptors);

        /* NOTE: "On Linux, select() modifies timeout to reflect the amount
        of time not slept; most other implementations don't do this" */
        ret = select(sockfd + 1, &descriptors, NULL, NULL, &timeout);
        if (ret < 0)
            perror("select");
        else if (!ret)
            return 0;

        ret = gettimeofday(&response->rec_rec_time, NULL);
        if (ret < 0)
            perror("gettimeofday");

        ret = recvfrom (sockfd, buffer, IP_MAXPACKET, MSG_DONTWAIT,
                (struct sockaddr*)&sender, &sender_len);
        if (ret < 0)
            perror("recvfrom");
    }  
    while (!verify_icmp_pack(buffer, min_seq, max_seq, getpid()));

    response->rec_icmp_type = get_icmp_type(buffer);
    response->rec_addr = sender.sin_addr;
    return 1;
}

int destination_reached(receive_t* responses, int num_send, int num_recv) {
    if (num_recv < num_send)
        return 0;
    for (int i = 0; i < num_recv; i++)
        if (responses[i].rec_icmp_type != ICMP_ECHOREPLY)
            return 0;
    return 1;
}
