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

#include "proto/simple_rpc.pb.h"
#include "rpc/rpc_channel.h"

using namespace libevrpc;
using namespace google::protobuf;
using namespace echo;

int main() {

    EchoRequest echo_request;
    echo_request.set_request("Hello! test");

    Channel rpc_channel("127.0.0.1", "9997");

    EchoService::Stub stub(&rpc_channel);

    EchoResponse echo_response;
    stub.Echo(NULL, &echo_request, &echo_response, NULL);

    echo_response.PrintDebugString();
    string ret = echo_response.response();
    printf("echo recv msg is %s\n", ret.c_str());

    return 0;
}






/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
