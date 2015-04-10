/***************************************************************************
 * 
 * Copyright (c) 2015 aishuyu, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file pub_define.h
 * @author aishuyu(asy5178@163.com)
 * @date 2015/03/06 15:15:46
 * @brief 
 *  
 **/




#ifndef  __PUB_DEFINE_H_
#define  __PUB_DEFINE_H_

namespace libevrpc {

#define SER_INTERNAL_ERROR 500
#define SER_RETURN_SUCC    200

// Be caution! if you chage MAX_INFO_LEN, MUST be note the send/recv max buf len
#define MAX_INFO_LEN 512


}  // end of namespace libevrpc







#endif  //__PUB_DEFINE_H_

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
