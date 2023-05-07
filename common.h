#ifndef COMMON_H
#define COMMON_H

#include<stdio.h>
#include<stdlib.h>

#define ERR_EXIT(msg) \
    do { \
        perror(msg); \
        exit(1); \
    } while(0)

#endif /* COMMON_H */
