/***************************************************************************
 *
 * Copyright (c) 2016 aishuyu, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file rpc_center.cpp
 * @author aishuyu(asy5178@163.com)
 * @date 2016/07/12 20:37:30
 * @brief
 *
 **/


#include "rpc_center.h"


namespace libevrpc {

RpcCenter::RpcCenter() {
}

RpcCenter::~RpcCenter() {
}

RpcCenter& RpcCenter::GetInstance() {
    static RpcCenter rc_instance;
    return rc_instance;
}


}  // end of namespace











/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
