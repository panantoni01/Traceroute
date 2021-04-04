#include"icmp.h"
#include"wrappers.h"

/* given an icmp header struct, computes its checksum; taken from lecture */
static uint16_t compute_icmp_checksum (const void *buff, int length){
	uint32_t sum;
	const uint16_t* ptr = buff;
	assert (length % 2 == 0);
	for (sum = 0; length > 0; length -= 2)
		sum += *ptr++;
	sum = (sum >> 16) + (sum & 0xffff);
	return (uint16_t)(~(sum + (sum >> 16)));
}

/* fill icmp structure with appropiate data and given sequence number*/
static void set_icmp_header(struct icmp* header, int ttl) {
    header->icmp_type = ICMP_ECHO;
    header->icmp_code = 0;
    /* we want to receive icmp packs only for this instance of traceroute: */
    header->icmp_hun.ih_idseq.icd_id = getpid();
    
    header->icmp_hun.ih_idseq.icd_seq = ttl;
    header->icmp_cksum = 0;
    header->icmp_cksum = compute_icmp_checksum((u_int16_t*)header, sizeof(*header));
}


/* send n icmp packs to given address with given ttl */
void send_icmp(int sockfd, struct sockaddr_in* address, int* ttl, int n) {
    /* prepare icmp header */
    struct icmp header;
    set_icmp_header(&header, *ttl);

    /* set appropiate ttl value */
    Setsockopt(sockfd, IPPROTO_IP, IP_TTL, ttl, sizeof(int));

    /* send icmp pack */
    for (int i = 0; i < n; i++)
        Sendto(sockfd, &header, sizeof(header), 0, 
            (struct sockaddr*) address, sizeof(*address));  
    
}

// ===========================================================

/* helper function for checking a good response was received */
static struct icmp* set_icmphdr_ptr (const void* buffer) {
    struct ip* ip_header = (struct ip*) buffer;
    uint8_t* packet = (uint8_t*)buffer + 4*ip_header->ip_hl;
    struct icmp* icmp_header = (struct icmp*) packet;
    
    if (icmp_header->icmp_type == ICMP_TIME_EXCEEDED) {
        struct ip* ip_header_inner = (struct ip*)((uint8_t*)icmp_header + sizeof(struct icmphdr));
        uint8_t* packet_inner = (uint8_t*)ip_header_inner + 4*ip_header_inner->ip_hl;
        icmp_header = (struct icmp*)packet_inner;
    }
    else if (icmp_header->icmp_type != ICMP_ECHOREPLY)
        return NULL;
    
    return icmp_header;
}

static int is_good_response(const void* buffer, int ttl) {
    struct icmp* icmp_header = set_icmphdr_ptr(buffer);
    if (icmp_header == NULL)
        return 0;
    
    uint16_t pack_id = icmp_header->icmp_hun.ih_idseq.icd_id;
    uint16_t pack_seq = icmp_header->icmp_hun.ih_idseq.icd_seq;

    uint16_t my_id = getpid();
    uint16_t my_seq = ttl;
    // printf("received id: %d\n", pack_id);
    // printf("received seq: %d\n", pack_seq);

    // printf("my id: %d\n", my_id);
    // printf("my seq: %d\n", my_seq);

    if ((pack_id != my_id) || (pack_seq != my_seq) )
        return 0;
    
    return 1;
}


int receive_icmp(int sockfd, int* ttl) {
    /* 1. wait 1s for icmp packets; receive them using select() and recvfrom()
       2. check if packets' icmp id and seq are ok (pid and ttl)
       3. compute checksum and check if ok
       4. print result depending on number of packs received:
        * 0 - print *
        * 1-2 - print router('s) ip('s) + '???' as we can't compute avg time
        * 3 - print router('s) ip('s) and compute avg rtt time
       5. if icmp_type was ECHO_REPLY - target was reached, so exit the program */

    /* function returns 1 if received packs from the target computer were received, 0 otherwise */
    int end_flag = 0;
    
    printf("%d. ", *ttl);
    
    // Ad 1
    fd_set descriptors;
    struct timeval tv;
    FD_ZERO (&descriptors);
    FD_SET (sockfd, &descriptors);
    tv.tv_sec = 1; tv.tv_usec = 0;
    int ready = Select(sockfd + 1, &descriptors, NULL, NULL, &tv);
 
    int good_packs = 0;

    for (int i = 0; i < ready; i++) {
        uint8_t buffer[IP_MAXPACKET];
        struct sockaddr_in sender;
        socklen_t sender_len = sizeof(sender);

        ssize_t packet_len = Recvfrom (sockfd, buffer, IP_MAXPACKET, MSG_DONTWAIT,
            (struct sockaddr*)&sender, &sender_len);

        // Ad 2 & 3
        if (!is_good_response(buffer, *ttl))
            continue;
        
        good_packs++;

        struct ip* ip_header = (struct ip*) buffer;
        uint8_t* packet = buffer + 4*ip_header->ip_hl;
        struct icmp* icmp_header = (struct icmp*) packet;
        if (icmp_header->icmp_type == ICMP_ECHOREPLY)
            end_flag = 1;

        char sender_ip_str[20]; 
		Inet_ntop(AF_INET, &(sender.sin_addr), sender_ip_str, sizeof(sender_ip_str));
        printf("%s ", sender_ip_str);

    }
    
    if (good_packs == 0)
        printf("*\n");
    else
        putchar('\n');
    return end_flag;
}