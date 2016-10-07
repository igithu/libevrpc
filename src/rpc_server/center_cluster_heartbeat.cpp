/***************************************************************************
 *
 * Copyright (c) 2016 aishuyu, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file center_cluster_heartbeat.cpp
 * @author aishuyu(asy5178@163.com)
 * @date 2016/09/07 19:44:39
 * @brief
 *
 **/

#include "center_cluster_heartbeat.h"

#include <unistd.h>
#include <time.h>
#include <fstream>
#include <iostream>


namespace libevrpc {

CenterClusterHeartbeat::CenterClusterHeartbeat() : center_addrs_ptr_(new ADDRS_LIST_TYPE()) {
}

CenterClusterHeartbeat::~CenterClusterHeartbeat() {
    if (NULL != center_addrs_ptr_) {
        delete center_addrs_ptr_;
    }
}

bool CenterClusterHeartbeat::InitCenterClusterHB() {
    const string cfile = "/tmp/centers.data";
    int ftyp = access(cfile.c_str(), 0);
    if (0 != ftyp) {
        /*
         * 需要的服务器列表文件不存在，无法初始化后与其他机器进行通信
         * 启动失败!!!
         */
        return false;
    }
    const char* center_port = config_parser_instance_.IniGetString("rpc_center:port", "8899");
    std::ifstream in(cfile);
    string line;
    while (getline (in, line)) {
        if (strcmp(line.c_str(), local_addr) == 0) {
            continue;
        }
        center_addrs_ptr_->push_back(line);
    }
    return true;
}

void CenterClusterHeartbeat::Run() {
    if (!InitCenterClusterHB()) {
        return;
    }
}


}  // end of namespace libevrpc
















/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
