#include"wrappers.h"

int Socket(int domain, int type, int protocol) {
    int sockfd; 
    if ((sockfd = socket(domain, type, protocol)) < 0)
        perror("socket");
    return sockfd;
}

int Inet_pton(int af, const char *src, void *dst) {
    int n = inet_pton(af, src, dst);
    if (n == 0)
        fprintf(stderr, "ERROR: invalid ip address!\n");
    else if (n < 0)
       perror("inet_pton");
    return n;
}

int Setsockopt(int sockfd, int level, int optname, 
                const void *optval, socklen_t optlen) {
    int n;
    if ((n = setsockopt(sockfd, level, optname, optval, optlen)) < 0)
        perror("setsockopt");
    return n;
}

ssize_t Sendto(int sockfd, const void *buf, size_t len, int flags, 
                const struct sockaddr *dest_addr, socklen_t addrlen) {
    
    ssize_t bytes = sendto(sockfd, buf, len, flags, 
        dest_addr, addrlen);
    if (bytes < 0)
        perror("sendto");
    return bytes;
}

const char* Inet_ntop(int af, const void *src,
                        char *dst, socklen_t size) {
    
    const char* ptr = inet_ntop(af, src, dst, size);
    if (ptr == NULL)
        perror("inet_ntop");
    return ptr;
}

ssize_t Recvfrom(int sockfd, void *buf, size_t len, int flags,
                        struct sockaddr *src_addr, socklen_t *addrlen) {
    ssize_t bytes = recvfrom(sockfd, buf, len, flags, src_addr, addrlen);
    if (bytes < 0)
        perror("recvfrom");
    return bytes;
}

int Select(int nfds, fd_set *readfds, fd_set *writefds,
                  fd_set *exceptfds, struct timeval *timeout) {
    
    int ret = select(nfds, readfds, writefds,
                  exceptfds, timeout);
    if (ret < 0)
        perror("select");
    return ret;
}

int Gettimeofday(struct timeval* tv, struct timezone* tz) {
    int ret = gettimeofday(tv, tz);
    if (ret < 0)
        perror("gettimeofday");
    return ret;
}
