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

#include <io.h>
#include <fstream>
#include <iostream>


namespace libevrpc {

using std::string;

RpcCenter::RpcCenter() :
    center_status_(LOOKING),
    other_centers_ptr_(new HashMap()){
}

RpcCenter::~RpcCenter() {
    if (NULL != other_centers_ptr_) {
        delete other_centers_ptr_;
    }
}

RpcCenter& RpcCenter::GetInstance() {
    static RpcCenter rc_instance;
    return rc_instance;
}

bool RpcCenter::InitRpcCenter() {
    const string cfile = "/tmp/centers.conf";
    int ftyp = _access(cfile.c_str(), 0);
    if (0 != ftyp) {
        /*
         * 需要的服务器列表文件不存在，无法初始化后与其他机器进行通信
         * 启动失败!!!
         */
        return false;
    }

    std::ifstream in(cfile);
    string line;
    while (getline (in, line)) {
        /*
         * 初始启动，视其他Center状态为LOOKING
         */
        other_centers_ptr_->put(line, LOOKING);
    }

    return true;
}


}  // end of namespace libevrpc











/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
