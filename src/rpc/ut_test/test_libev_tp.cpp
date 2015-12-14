/***************************************************************************
 *
 * Copyright (c) 2015 aishuyu, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file test_libev_tp.cpp
 * @author aishuyu(asy5178@163.com)
 * @date 2015/12/13 22:51:29
 * @brief
 *
 **/

#include "../libev_thread_pool.h"

using namespace libevrpc;

int main() {
    LibevThreadPool ltp;
    ltp.Start();

    return 0;
}





/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
