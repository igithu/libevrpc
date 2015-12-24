/***************************************************************************
 * 
 * Copyright (c) 2014 Aishuyu. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file rpc_client.cpp
 * @author aishuyu(asy5178@163.com)
 * @date 2014/12/02 14:41:35
 * @brief 
 *  
 **/

#include <stdio.h>

#include "proto/rpc_sample.pb.h"
#include "rpc_client/rpc_client.h"

using namespace libevrpc;
using namespace google::protobuf;
using namespace echo;

class RpcClientImp : public RpcClient {
    public:
        RpcClientImp();

        ~RpcClientImp();

        EchoService_Stub& RpcCall();

    private:
        EchoService_Stub* service_call_ptr_;
};

RpcClientImp::RpcClientImp() {
    InitClient("127.0.0.1", "9999");
    service_call_ptr_ = new EchoService_Stub(GetRpcChannel());
}

RpcClientImp::~RpcClientImp() {
}

EchoService_Stub& RpcClientImp::RpcCall() {
    return (*service_call_ptr_);
}

void SysncCall() {

    EchoRequest echo_request;
    echo_request.set_request("Hello! test123456789012345678901234567890");

    EchoResponse echo_response;
    RpcClientImp rpc_client;
    rpc_client.RpcCall().Echo(NULL, &echo_request, &echo_response, NULL);;
    echo_response.PrintDebugString();
    string ret = echo_response.response();
    printf("echo recv msg is %s\n", ret.c_str());

}

void AsyscCall() {
    EchoRequest echo_request;
    echo_request.set_request("Hello! test123456789012345678901234567890");

    RpcClientImp rpc_client;
    rpc_client.OpenRpcAsyncMode();
    EchoResponse echo_response;
    rpc_client.RpcCall().Echo(NULL, &echo_request, &echo_response, NULL);

    sleep(5);
    rpc_client.RpcClient::GetAsyncResponse("Echo", &echo_response);
    echo_response.PrintDebugString();
    string ret = echo_response.response();
    printf("Async call echo recv msg is %s\n", ret.c_str());

}

int main() {
    SysncCall();
//    AsyscCall();
    return 0;
}






/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
