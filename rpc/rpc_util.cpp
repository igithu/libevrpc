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

namespace libevrpc {

uint32_t BKDRHash(const char *orig_str) {

    uint32_t seed = 1313;
    uint32_t hash_code = 0;

    while (*orig_str) {
        hash_code = hash_code * seed + (*orig_str++);
    }

    return (hash_code & 0x7FFFFFFF);
}

}  // end of namespace libevrpc





/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
