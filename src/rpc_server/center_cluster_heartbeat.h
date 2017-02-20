/***************************************************************************
 *
 * Copyright (c) 2016 aishuyu, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file center_cluster_heartbeat.h
 * @author aishuyu(asy5178@163.com)
 * @date 2016/09/07 20:43:13
 * @brief
 *
 **/




#ifndef __CENTER_CLUSTER_HEARTBEAT_H
#define __CENTER_CLUSTER_HEARTBEAT_H

#include <vector>
#include <string>

#include "util/thread.h"

namespace libevrpc {

typedef std::vector<std::string> ADDRS_LIST_TYPE;

class CenterClusterHeartbeat : public Thread {
    public:
        CenterClusterHeartbeat(const std::string& config_file);
        ~CenterClusterHeartbeat();

        bool InitCenterClusterHB();

        virtual void Run();

    private:
        std::string config_file_;
        ADDRS_LIST_TYPE* center_addrs_ptr_;
        ADDRS_LIST_TYPE* reporter_center_addrs_ptr_;

        bool running_;
        char* center_port_;
};

}  // end of namespace libevrpc






#endif // __CENTER_CLUSTER_HEARTBEAT_H



/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
