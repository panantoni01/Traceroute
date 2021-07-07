#include"wrappers.h"

int Socket(int domain, int type, int protocol) {
    int sockfd; 
    if ((sockfd = socket(domain, type, protocol)) < 0) {
        fprintf(stderr, "socket error: %s\n", strerror(errno));
        exit(1);
    }
    return sockfd;
}

int Inet_pton(int af, const char *src, void *dst) {
    int n;
    if ((n = inet_pton(af, src, dst)) <= 0) {
        if (n == 0)
            fprintf(stderr, "ERROR: invalid ip address!\n");
        else 
            fprintf(stderr, "inet_pton error: %s\n", strerror(errno));
        exit(1);
    }
    return n;
}

int Setsockopt(int sockfd, int level, int optname, 
                const void *optval, socklen_t optlen) {
    int n;
    if ((n = setsockopt(sockfd, level, optname, optval, optlen)) < 0) {
        fprintf(stderr, "setsockopt error: %s\n", strerror(errno));
        exit(1);
    }
    return n;
}

ssize_t Sendto(int sockfd, const void *buf, size_t len, int flags, 
                const struct sockaddr *dest_addr, socklen_t addrlen) {
    
    ssize_t bytes = sendto(sockfd, buf, len, flags, 
        dest_addr, addrlen);
    if (bytes < 0) {
        fprintf(stderr, "sendto error: %s\n", strerror(errno));
        exit(1);
    }
    return bytes;
}

const char* Inet_ntop(int af, const void *src,
                        char *dst, socklen_t size) {
    
    const char* ptr = inet_ntop(af, src, dst, size);
    if (ptr == NULL) {
        fprintf(stderr, "inet_ntop error: %s\n", strerror(errno));
        exit(1);
    }
    return ptr;
}

ssize_t Recvfrom(int sockfd, void *buf, size_t len, int flags,
                        struct sockaddr *src_addr, socklen_t *addrlen) {
    ssize_t bytes = recvfrom(sockfd, buf, len, flags, src_addr, addrlen);
    if (bytes < 0) {
        fprintf(stderr, "recvfrom error: %s\n", strerror(errno)); 
		exit(1);
    }
    return bytes;
}

int Select(int nfds, fd_set *readfds, fd_set *writefds,
                  fd_set *exceptfds, struct timeval *timeout) {
    
    int ret = select(nfds, readfds, writefds,
                  exceptfds, timeout);
    if (ret < 0) {
        fprintf(stderr, "select error: %s\n", strerror(errno)); 
		exit(1);
    }
    return ret;
}

int Getaddrinfo(const char *restrict node, const char *restrict service,
                const struct addrinfo *restrict hints, struct addrinfo **restrict res) {
    
    int ret = getaddrinfo(node, service, hints, res);            
    if (ret != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(ret));
        exit(1);
    }
    return ret;                      
}

int Getnameinfo(const struct sockaddr *restrict addr, socklen_t addrlen,
                       char *restrict host, socklen_t hostlen,
                       char *restrict serv, socklen_t servlen, int flags) {
    int ret = getnameinfo(addr, addrlen, 
                            host, hostlen, 
                            serv, servlen, flags);
    if (ret != 0 && ret != EAI_AGAIN) {
        fprintf(stderr, "getnameinfo error: %s\n", gai_strerror(ret));
        exit(1);
    }
    return ret;   
}