#include<string.h>
#include<netinet/ip.h>
#include<netinet/ip_icmp.h>
#include<linux/errqueue.h>
#include<assert.h>

#include"udp.h"
#include"common.h"
#include"report.h"


void send_udp_probe(int sockfd, struct sockaddr_in* address, int ttl) {
    const char data[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(int)) < 0)
        eprintf("setsockopt:");

    if (sendto(sockfd, data, strlen(data), 0,
        (struct sockaddr *) address, sizeof(*address)) < 0)
        eprintf("sendto:");
}

/* Verify whether a single received icmp package in `cmsg` is a response to
some previously sent UDP probe. */
static unsigned int verify_icmp(struct cmsghdr* cmsg) {
    /* TODO - make some reserach whether it's better to use memcpy(3) here */
    struct sock_extended_err* sock_err = (struct sock_extended_err*)(CMSG_DATA(cmsg));

    /* Check whether we deal with some ICMP error response  */
    if (cmsg->cmsg_level != IPPROTO_IP ||
        cmsg->cmsg_type != IP_RECVERR ||
        sock_err->ee_origin != SO_EE_ORIGIN_ICMP)
        return 0;
    
    /* 1. OK if we got "TTL exceeded" ICMP error */
    if (sock_err->ee_type == ICMP_TIME_EXCEEDED)
        return 1;

    /* 2. OK if we got "destination port unreachable" ICMP error */
    if (sock_err->ee_type == ICMP_DEST_UNREACH) {
        if (sock_err->ee_code == ICMP_PORT_UNREACH)
            return 1;
        else
            fprintf(stderr, "Received ICMP_DEST_UNREACH"
                            "error with code: %d\n", (int)sock_err->ee_code);
    }

    return 0;
}

/* Fill in `response` structure with the data coming from an ICMP error message
stored in an errqueue in struct sock_extended_err of `cmsg`. */
static void gather_response_data(struct cmsghdr* cmsg, receive_t* response) {
    struct sock_extended_err* sock_err = (struct sock_extended_err*)(CMSG_DATA(cmsg));
    struct sockaddr_in* offender = (struct sockaddr_in*)(SO_EE_OFFENDER(sock_err));

    assert(sock_err->ee_type == ICMP_TIME_EXCEEDED ||
            (sock_err->ee_type == ICMP_DEST_UNREACH && sock_err->ee_code == ICMP_PORT_UNREACH));
    
    if (gettimeofday(&response->rec_rec_time, NULL) < 0)
        eprintf("gettimeofday:");
    
    response->rec_addr = offender->sin_addr;
    response->rec_icmp_type = sock_err->ee_type;
}

/* Receive icmps from `sockfd` that correspond to some previously sent UDP
probes. Store statistics in `responses`.

Return number of received icmp packages */
static unsigned int receive_icmps(
    int sockfd,
    receive_t* responses,
    config_t* config) {

    ssize_t _ret;
    int num_recv = 0;
    fd_set descriptors;
    struct timeval timeout = config->wait_time;

    struct msghdr msg;
    struct cmsghdr* cmsg;
    struct sockaddr_in msg_name;
    struct iovec iov;
    uint8_t msg_control_buf[512];
    uint8_t iov_base[IP_MAXPACKET];

    msg.msg_name = &msg_name;
    msg.msg_namelen = sizeof(msg_name);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_control = &msg_control_buf;
    msg.msg_controllen = sizeof(msg_control_buf);
    iov.iov_base = &iov_base;
    iov.iov_len = sizeof(iov_base);

    do {
        FD_ZERO(&descriptors);
        FD_SET(sockfd, &descriptors);

        _ret = select(sockfd + 1, &descriptors, NULL, NULL, &timeout);
        if (_ret < 0)
            eprintf("select:");
        else if (_ret == 0)
            break;
        
        if (recvmsg(sockfd, &msg, MSG_ERRQUEUE) < 0)
            eprintf("recvmsg:");
        
        for (cmsg = CMSG_FIRSTHDR(&msg); cmsg != NULL;
                   cmsg = CMSG_NXTHDR(&msg, cmsg))
            if (verify_icmp(cmsg))
                gather_response_data(cmsg, &responses[num_recv++]);
    }
    while (num_recv < config->num_send);

    return num_recv;
}


void udp_main(config_t* config) {
    int sockfd, recverr = 1, ttl, i, num_received;
    struct sockaddr_in send_address = config->address;
    uint16_t dest_port = config->dest_port;

    for (ttl = config->first_ttl; ttl <= config->max_ttl; ttl++) {
        receive_t responses[config->num_send];
        memset(responses, 0, config->num_send * sizeof(receive_t));

        if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
            eprintf("socket:");
    
        if (setsockopt(sockfd, SOL_IP, IP_RECVERR, &recverr, sizeof(recverr)) < 0)
            eprintf("setsockopt:");

        for (i = 0; i < config->num_send; i++) {
            send_address.sin_port = htons(dest_port++);
            send_udp_probe(sockfd, &send_address, ttl);

            if (gettimeofday(&responses[i].rec_send_time, NULL) < 0)
                eprintf("gettimeofday:");
        }

        num_received = receive_icmps(sockfd, responses, config);

        print_report(ttl, responses, config->num_send, num_received, config->use_dns);
    }
}
