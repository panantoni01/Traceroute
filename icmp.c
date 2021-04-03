#include"icmp.h"
#include"wrappers.h"

/* given an icmp header struct, computes its checksum; taken from lecture */
uint16_t compute_icmp_checksum (const void *buff, int length){
	uint32_t sum;
	const uint16_t* ptr = buff;
	assert (length % 2 == 0);
	for (sum = 0; length > 0; length -= 2)
		sum += *ptr++;
	sum = (sum >> 16) + (sum & 0xffff);
	return (uint16_t)(~(sum + (sum >> 16)));
}

void set_icmp_header(struct icmp* header, int seq) {
    header->icmp_type = ICMP_ECHO;
    header->icmp_code = 0;
    header->icmp_hun.ih_idseq.icd_id = getpid();
    header->icmp_hun.ih_idseq.icd_seq = seq;
    header->icmp_cksum = 0;
    header->icmp_cksum = compute_icmp_checksum((u_int16_t*)header, sizeof(*header));
}


/* send single icmp pack to given address with given ttl and seq number */
void send_icmp(int sockfd, struct sockaddr_in* address, int* ttl, int seq) {
    /* prepare icmp header */
    struct icmp header;
    set_icmp_header(&header, seq);

    /* set appropiate ttl value */
    Setsockopt(sockfd, IPPROTO_IP, IP_TTL, ttl, sizeof(int));

    /* send icmp ping */
    Sendto(sockfd, &header, sizeof(header), 0, 
        (struct sockaddr*) address, sizeof(*address));
    
    char ip_addr[20]; 
	Inet_ntop(AF_INET, &(address->sin_addr), ip_addr, sizeof(ip_addr));
    printf("ICMP pack sent to: %s\n", ip_addr);

}

void receive_icmp(int sockfd, struct sockaddr_in* address) {
    uint8_t buffer[IP_MAXPACKET];
    socklen_t address_len = sizeof(*address);
    
	Recvfrom(sockfd, buffer, IP_MAXPACKET, 0, (struct sockaddr*) address, &address_len);

    char ip_str[20]; 
	Inet_ntop(AF_INET, &(address->sin_addr), ip_str, sizeof(ip_str));
	printf("Received IP packet from: %s\n", ip_str);
}