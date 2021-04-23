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
    header->icmp_hun.ih_idseq.icd_id = htons(getpid());
    
    header->icmp_hun.ih_idseq.icd_seq = htons(ttl);
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

    /* send icmp packs */
    for (int i = 0; i < n; i++)
        Sendto(sockfd, &header, sizeof(header), 0, 
            (struct sockaddr*) address, sizeof(*address));  
    
}

// ===========================================================

/* helper function for checking if a good response was received */
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
    
    /* returns NULL if bad icmp type or a pointer to:
    * icmphdr if type is ICMP_ECHOREPLY
    * inner icmphdr, that is in payload of icmp packet if type is ICMP_TIME_EXCEEDED */
    return icmp_header;
}

/* check if received icmp package is a response to some previously 
sent icmp echo request (for given ttl) */
static int is_good_response(const void* buffer, int ttl) {
    struct icmp* icmp_header = set_icmphdr_ptr(buffer);
    
    if (icmp_header == NULL)
        return 0;

    if ((ntohs(icmp_header->icmp_hun.ih_idseq.icd_id) != getpid()) || 
        (ntohs(icmp_header->icmp_hun.ih_idseq.icd_seq) != ttl))
        return 0;
    
    return 1;
}

/* self - explanatory */
static int is_echoreply(const void* buffer){
    struct ip* ip_header = (struct ip*) buffer;
    uint8_t* packet = (uint8_t*)buffer + 4*ip_header->ip_hl;
    struct icmp* icmp_header = (struct icmp*) packet;
    if (icmp_header->icmp_type == ICMP_ECHOREPLY)
        return 1;
    return 0;
}

static int await_single_pack(int sockfd, struct timeval* tv) {
    fd_set descriptors;
    FD_ZERO (&descriptors);
    FD_SET (sockfd, &descriptors);

    return Select(sockfd + 1, &descriptors, NULL, NULL, tv);
}  

int receive_icmp(int sockfd, int* ttl, int n) {
    int end_flag = 0;
    int good_packs = 0;
    double time_elapsed = 0;
    /* arrays for distinct ip addresses, if they occur
    (usually we print only 1 ip addr... but sometimes more!) */
    in_addr_t ip_addrs[n];
    int ip_count = 0;
    
    /* this structure will be processed in Select() syscall */
    struct timeval tv;
    tv.tv_sec = 1; tv.tv_usec = 0;

    while ((good_packs < n) && await_single_pack(sockfd, &tv)) {
        uint8_t buffer[IP_MAXPACKET];
        struct sockaddr_in sender;
        socklen_t sender_len = sizeof(sender);

        Recvfrom (sockfd, buffer, IP_MAXPACKET, MSG_DONTWAIT,
            (struct sockaddr*)&sender, &sender_len);

        if (!is_good_response(buffer, *ttl))
            continue;

        /* measure time spent on receiving this package */
        struct timeval tv_end;
        tv_end.tv_sec = 1; tv_end.tv_usec = 0;
        timersub(&tv_end, &tv, &tv_end);
        time_elapsed += tv_end.tv_usec;

        good_packs++;

        if (is_echoreply(buffer))
            end_flag = 1;

        /* if the sender ip address has already occurred, dont add it to ip_addrs[];
        otherwise we need to add it, so that it will be printed later*/
        int j = 0;
        while (j < ip_count) {
            if (sender.sin_addr.s_addr == ip_addrs[j])
                break;
            j++;
        }
        if (j == ip_count) {
            ip_addrs[j] = sender.sin_addr.s_addr;
            ip_count++;
        }
    }
    
    printf("%d. ", *ttl);
    if (good_packs == 0)
        printf("*\n");
    else {
        for (int j = 0; j < ip_count; j++) {
            char printable_addr[20] = {'\0'};
            Inet_ntop(AF_INET, ip_addrs + j, printable_addr, sizeof(printable_addr));
            printf("%s ", printable_addr);
        }
        if (good_packs < n)
            printf("???");
        else 
            printf("%.3fms", (time_elapsed / 1000) / n);
        putchar('\n');
    }
    
    /* function returns 1 if packs from the "target" computer were received, 0 otherwise */
    return end_flag;
}