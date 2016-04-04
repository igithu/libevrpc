/***************************************************************************
 *
 * Copyright (c) 2015 aishuyu, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file rpc_heartbeat_client.h
 * @author aishuyu(asy5178@163.com)
 * @date 2016/04/04 21:04:43
 * @brief
 *
 **/




#ifndef __RPC_HEARTBEAT_CLIENT_H
#define __RPC_HEARTBEAT_CLIENT_H


#include "util/thread.h"

namespace libevrpc {

class RpcHeartbeatClient : public Thread {
    public:
        RpcHeartbeatClient();
        ~RpcHeartbeatClient();

        int32_t InitRpcConnection();
        virtual void Run();

    private:
        char* hb_server_addr_;
        char* hb_server_port_;

        volatile bool running_;
};

}  // end of namespace libevrpc






#endif // __RPC_HEARTBEAT_CLIENT_H



/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
