/***************************************************************************
 * 
 * Copyright (c) 2014 Aishuyu. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file rpc_util.h
 * @author aishuyu(asy5178@163.com)
 * @date 2014/11/30 17:47:28
 * @brief 
 *  
 **/




#ifndef  __RPC_UTIL_H_
#define  __RPC_UTIL_H_

#include <stdint.h>

namespace libevrpc {

uint32_t BKDRHash(const char *orig_str);

}  // end of namespace libevrpc



#endif  //__RPC_UTIL_H_

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
