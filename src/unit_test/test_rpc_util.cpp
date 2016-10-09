/***************************************************************************
 *
 * Copyright (c) 2016 aishuyu, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file test_rpc_util.cpp
 * @author aishuyu(asy5178@163.com)
 * @date 2016/04/12 20:18:24
 * @brief
 *
 **/

#include <stdio.h>

#include <string>

#include "../util/rpc_util.h"

using namespace libevrpc;


int main() {

    PrintErrorInfo("Test");

    std::string ip_addr = "10.189.192.213";
    printf("out test\n");

    uint32_t hash_id = MurMurHash2(ip_addr.c_str(), ip_addr.size());

    printf("the hash id is %u", hash_id);

    return 0;
}








/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
