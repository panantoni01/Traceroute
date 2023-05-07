#ifndef UDP_H
#define UDP_H

#include <arpa/inet.h>
#include <sys/time.h>

#include "common.h"

void send_udp_probe(int sendfd, struct sockaddr_in* address, int ttl);

#endif /* !UDP_H */
