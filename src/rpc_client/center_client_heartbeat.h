/***************************************************************************
 *
 * Copyright (c) 2016 aishuyu, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file center_client_heartbeat.h
 * @author aishuyu(asy5178@163.com)
 * @date 2016/09/07 20:52:26
 * @brief
 *
 **/




#ifndef __CENTER_CLIENT_HEARTBEAT_H
#define __CENTER_CLIENT_HEARTBEAT_H

#include <vector>
#include <string>

#include <google/protobuf/repeated_field.h>

#include "util/thread.h"
#include "util/pthread_rwlock.h"

namespace libevrpc {

typedef std::vector<std::string> ADDRS_LIST_TYPE;

class CenterClientHeartbeat : public Thread {
    public:
        CenterClientHeartbeat(const std::string& config_file);

        ~CenterClientHeartbeat();

        virtual void Run();

        void GetRpcServerList(ADDRS_LIST_TYPE& server_list);
        std::string RandomGetRpcServerAddr();

    private:
        bool InitCenterClientHB();

        void UpdateCenterAddrs(const ::google::protobuf::RepeatedPtrField<std::string>* center_list);
        void UpdateServerAddrs(const ::google::protobuf::RepeatedPtrField<std::string>* server_list);

    private:
        std::string config_file_;
        ADDRS_LIST_TYPE* center_addrs_ptr_;
        ADDRS_LIST_TYPE* updatecenter_addrs_ptr_;
        ADDRS_LIST_TYPE* cluster_server_addrs_list_ptr_;

        bool running_;
        char* center_port_;

        RWLock centers_rwlock_;
        RWLock servers_rwlock_;
};

}  // end of namespace libevrpc




#endif // __CENTER_CLIENT_HEARTBEAT_H



/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
