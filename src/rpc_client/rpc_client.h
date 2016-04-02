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

#include <string>

namespace libevrpc {

class RpcClient {
    public:
        RpcClient();
        virtual ~RpcClient();

        bool OpenRpcAsyncMode();
        bool GetAsyncResponse(const std::string& method_name, google::protobuf::Message* response);
        bool SetRpcConnectionInfo(int32_t rpc_timeout, int32_t try_time = 1);
        google::protobuf::RpcController* Status();

        bool IsCallOk();
        std::string GetErrorInfo() const;

    protected:
        bool InitClient(const char* addr = "127.0.0.1", const char* port = "8899");

        Channel* GetRpcChannel();

    private:
        Channel* rpc_channel_ptr_;
        google::protobuf::RpcController* rpc_controller_ptr_;



};

}  // end of namespace libevrpc



#endif // __RPC_CLIENT_H



/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
