/***************************************************************************
 *
 * Copyright (c) 2015 aishuyu, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file rpc_heartbeat_server.h
 * @author aishuyu(asy5178@163.com)
 * @date 2016/04/04 22:08:26
 * @brief
 *
 **/




#ifndef __RPC_HEARTBEAT_SERVER_H
#define __RPC_HEARTBEAT_SERVER_H

#include "util/thread.h"

namespace libevrpc {

class RpcHeartbeatServer : public Thread {
    public:
        RpcHeartbeatServer();
        ~RpcHeartbeatServer();

        void Intit();
        virtual void Run();

    private:
        char* hb_host_;
        char* hb_port_;
};

}  // end of namespace libevrpc




#endif // __RPC_HEARTBEAT_SERVER_H



/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
