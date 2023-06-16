#include<stdio.h>
#include<sys/time.h>
#include<arpa/inet.h>
#include<string.h>
#include<netdb.h>

#include "report.h"
#include "icmp.h"
#include "common.h"


static char* reverse_dns_lookup(struct in_addr *ip_addr, char hostname[NI_MAXHOST]) {
    struct sockaddr_in address = { 
        .sin_addr = *ip_addr,
        .sin_family = AF_INET
    };

    if (getnameinfo((struct sockaddr*) &address, sizeof(struct sockaddr_in), hostname, NI_MAXHOST, NULL, 0, 0))
        return NULL;
    return hostname;
}

static void print_ip_addrs(receive_t* responses, int num_recv, int use_dns) {
    int i, j, num_addrs = 0;
    struct in_addr distinct_addrs[num_recv];
    char ip_addr_buf[INET_ADDRSTRLEN];
    char hostname[NI_MAXHOST];

    for (i = 0; i < num_recv; i++) {       
        for (j = 0; j < num_addrs; j++) {
            if (responses[i].rec_addr.s_addr == distinct_addrs[j].s_addr)
                break;
        }
        if (j == num_addrs)
            distinct_addrs[num_addrs++] = responses[i].rec_addr;
    }
    for (i = 0; i < num_addrs; i++) {
        memset(ip_addr_buf, 0, sizeof(ip_addr_buf));
        if (!inet_ntop(AF_INET, &(distinct_addrs[i]), ip_addr_buf, INET_ADDRSTRLEN))
            eprintf("inet_ntop:");
        if (!use_dns)
            printf(" %s", ip_addr_buf);
        else if (reverse_dns_lookup(&(distinct_addrs[i]), hostname))
            printf(" %s (%s)", hostname, ip_addr_buf);
        else
            printf(" %s", ip_addr_buf);
    }
}

static void print_avg_time(receive_t* responses, int num_recv) {
    int i;
    long elapsed_us = 0;
    struct timeval tv;

    for (i = 0; i < num_recv; i++) {
        timersub(&responses[i].rec_rec_time, &responses[i].rec_send_time, &tv);
        elapsed_us += tv.tv_usec + 1e6*tv.tv_sec;
    } 
    printf("%.3fms", (double)elapsed_us / 1000 / num_recv);
}

void print_report(int ttl, receive_t* responses, int num_send, int num_recv, int use_dns) {
    if (num_recv == 0) {
        printf("%d. *\n", ttl);
        return;
    }

    printf("%d.", ttl);

    print_ip_addrs(responses, num_recv, use_dns);

    printf("  ");

    if (num_send < num_recv)
        printf("???");
    else
        print_avg_time(responses, num_recv);
    
    putchar('\n');
}
