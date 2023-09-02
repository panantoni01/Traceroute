#ifndef REPORT_H
#define REPORT_H

#include "icmp.h"
#include "common.h"


void print_report(int ttl, receive_t *responses, int num_recv);

#endif /* !REPORT_H */
