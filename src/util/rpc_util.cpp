/***************************************************************************
 *
 * Copyright (c) 2014 Aishuyu. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file rpc_util.cpp
 * @author aishuyu(asy5178@163.com)
 * @date 2014/11/30 17:49:07
 * @brief
 *
 **/

#include "rpc_util.h"

#include <cstddef>
#include <stdio.h>
#include <time.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

namespace libevrpc {

uint32_t BKDRHash(const char *orig_str) {
    uint32_t seed = 1313;
    uint32_t hash_code = 0;

    while (*orig_str) {
        hash_code = hash_code * seed + (*orig_str++);
    }

    return (hash_code & 0x7FFFFFFF);
}

void PrintErrorInfo(const char* error_info) {
    time_t nowtime = time(NULL);
    struct tm *local = localtime(&nowtime);

    char time[30];
    strftime(time, 30, "%Y-%m-%d %H:%M:%S", local);
    fprintf(stderr, "%s [] %s\n", time, error_info);
}

const char* GetLocalAddress() {
    char hname[128];
    gethostname(hname, sizeof(hname));
    struct hostent *hptr = gethostbyname(hname);
    const char* addr = inet_ntoa(*(struct in_addr*)(hptr->h_addr_list[0]));
    return addr;
}


uint32_t MurMurHash2(const char* key_str, int32_t len) {
    const unsigned int m = 0x5bd1e995;
    const int r = 24;
    const int seed = 97;
    unsigned int h = seed ^ len;

    // Mix 4 bytes at a time into the hash
    const unsigned char *data = (const unsigned char *)key;
    while (len >= 4) {
        unsigned int k = *(unsigned int *)data;
        k *= m;
        k ^= k >> r;
        k *= m;
        h *= m;
        h ^= k;
        data += 4;
        len -= 4;
    }

    switch (len) {
        case 3: h ^= data[2] << 16;
        case 2: h ^= data[1] << 8;
        case 1: h ^= data[0];
                h *= m;
    }
    // Do a few final mixes of the hash to ensure the last few
    // bytes are well-incorporated.
    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;
    return h;
}

}  // end of namespace libevrpc





/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
