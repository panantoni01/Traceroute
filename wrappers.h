#ifndef WRAPPERS_H
#define WRAPPERS_H

#include<stdio.h>
#include<errno.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<string.h>
#include<sys/time.h>
#include<netdb.h>


int Socket(int domain, int type, int protocol);
int Inet_pton(int af, const char *src, void *dst);
int Setsockopt(int sockfd, int level, int optname, 
                const void *optval, socklen_t optlen);
ssize_t Sendto(int sockfd, const void *buf, size_t len, int flags, 
                const struct sockaddr *dest_addr, socklen_t addrlen);
const char* Inet_ntop(int af, const void *src,
                             char *dst, socklen_t size);
ssize_t Recvfrom(int sockfd, void *buf, size_t len, int flags,
                        struct sockaddr *src_addr, socklen_t *addrlen);
int Select(int nfds, fd_set *readfds, fd_set *writefds,
                  fd_set *exceptfds, struct timeval *timeout);
int Gettimeofday(struct timeval *tv, struct timezone *tz);
int Getaddrinfo(const char *restrict node, const char *restrict service,
                const struct addrinfo *restrict hints, struct addrinfo **restrict res);
int Getnameinfo(const struct sockaddr *restrict addr, socklen_t addrlen,
                       char *restrict host, socklen_t hostlen,
                       char *restrict serv, socklen_t servlen, int flags);

#endif /* !WRAPPERS_H */