/***************************************************************************
 *
 * Copyright (c) 2015 aishuyu, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file rpc_client.h
 * @author aishuyu(asy5178@163.com)
 * @date 2015/09/10 17:43:49
 * @brief
 *
 **/




#ifndef __RPC_CLIENT_H
#define __RPC_CLIENT_H

#include "rpc_channel.h"

namespace libevrpc {

using namespace google::protobuf;

class RpcClient {

    public:
        RpcClient();

        virtual ~RpcClient();

    protected:
        bool OpenRpcAsyncMode();

        bool InitClient(const char* addr = "127.0.0.1", const char* port = "8899");

        Channel* GetRpcChannel();

        // Please use the ServerCall to call the rpc function!
        void* ServerCall();

    private:
        Channel* rpc_channel_ptr_;

        void* service_call_ptr_;

        bool is_async_;

};

}  // end of namespace libevrpc



#endif // __RPC_CLIENT_H



/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
