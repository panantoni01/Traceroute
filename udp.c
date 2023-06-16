#include<string.h>

#include"udp.h"
#include"common.h"


void send_udp_probe(int sendfd, struct sockaddr_in* address, int ttl) {
    const char data[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    if (setsockopt(sendfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(int)) < 0)
        eprintf("setsockopt:");

    if (sendto(sendfd, data, strlen(data),  MSG_CONFIRM,
        (struct sockaddr *) address, sizeof(*address)) < 0)
        eprintf("sendto:");
}


void udp_main(config_t* config) {
    int sendfd, ttl, i;
    uint16_t dest_port = config->dest_port;

    sendfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sendfd < 0)
        eprintf("socket:");
    
    for (ttl = config->first_ttl; ttl <= config->max_ttl; ttl++) {
        receive_t responses[config->num_send];
        memset(responses, 0, config->num_send * sizeof(receive_t));

        for (i = 0; i < config->num_send; i++) {
            config->address.sin_port = htons(dest_port++);
            send_udp_probe(sendfd, &config->address, ttl);
        }
        
        /* TODO: receive packets
           1. receive ICMP TTL_EXC or DEST_UNREACHABLE messages 
           2. print report
           3. check if destination reached 
        */
    }
}