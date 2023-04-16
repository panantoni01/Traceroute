#ifndef REPORT_H
#define REPORT_H

#include "icmp.h"


void print_report(int ttl, receive_t* responses, int num_send, int num_recv);

#endif /* !REPORT_H */
