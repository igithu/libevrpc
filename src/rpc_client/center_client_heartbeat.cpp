/***************************************************************************
 *
 * Copyright (c) 2016 aishuyu, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file center_client_heartbeat.cpp
 * @author aishuyu(asy5178@163.com)
 * @date 2016/09/07 20:59:44
 * @brief
 *
 **/


#include "center_client_heartbeat.h"

#include <unistd.h>
#include <time.h>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <sys/sysinfo.h>
#include <linux/kernel.h>

#include <google/protobuf/repeated_field.h>

#include "center_proto/center_cluster.pb.h"
#include "config_parser/config_parser.h"
#include "util/rpc_communication.h"
#include "util/rpc_util.h"


namespace libevrpc {

CenterClientHeartbeat::CenterClientHeartbeat(const string& config_file):
    config_file_(config_file),
    center_addrs_ptr_(new ADDRS_LIST_TYPE()),
    updatecenter_addrs_ptr_(new ADDRS_LIST_TYPE()),
    running_(false),
    center_port_(NULL){
}

CenterClientHeartbeat::~CenterClientHeartbeat() {
    if (NULL != center_addrs_ptr_) {
        delete center_addrs_ptr_;
    }

    if (NULL != updatecenter_addrs_ptr_) {
        delete updatecenter_addrs_ptr_;
    }

    if (NULL != center_port_) {
        free(center_port_);
    }
}

void CenterClientHeartbeat::Run() {
}

bool CenterClientHeartbeat::InitCenterClientHB() {
    const string cfile = "/tmp/centers.data";
    int ftyp = access(cfile.c_str(), 0);
    if (0 != ftyp) {
        /*
         * 需要的服务器列表文件不存在，无法初始化后与其他机器进行通信
         * 启动失败!!!
         */
        return false;
    }
    const char* center_port = ConfigParser::GetInstance(config_file_).IniGetString("rpc_center:port", "8899");
    strcpy(center_port_ = (char*)malloc(strlen(center_port) + 1), center_port);

    std::ifstream in(cfile);
    string line;

    const char* local_addr = GetLocalAddress();
    while (getline (in, line)) {
        if (strcmp(line.c_str(), local_addr) == 0) {
            continue;
        }
        center_addrs_ptr_->push_back(line);
    }
    int32_t random_index = random(center_addrs_ptr_->size());
    int32_t conn_fd = TcpConnect(center_addrs_ptr_->at(random_index).c_str(), center_port_, 15);
    if (conn_fd <= 0) {
        return false;
    }

    running_ = true
    return true;
}


}  // end of namespace libevrpc









/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
