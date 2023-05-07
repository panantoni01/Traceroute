#include "udp.h"


void send_udp_probe(int sendfd, struct sockaddr_in* address, int ttl) {
    const char data[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    if (setsockopt(sendfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(int)) < 0)
        ERR_EXIT("setsockopt");

    if (sendto(sendfd, data, strlen(data),  MSG_CONFIRM,
        (struct sockaddr *) address, sizeof(*address)) < 0)
        ERR_EXIT("sendto");
}
