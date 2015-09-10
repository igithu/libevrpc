/***************************************************************************
 * 
 * Copyright (c) 2014 Aishuyu. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file rpc_server.cc
 * @author aishuyu(asy5178@163.com)
 * @date 2014/12/02 17:01:36
 * @brief 
 *  
 **/



#include "proto/rpc_sample.pb.h"
#include "rpc/rpc_server.h"

using namespace libevrpc;
using namespace echo;

class EchoServiceImpl : public EchoService {
    public:
        EchoServiceImpl() {
        }

        ~EchoServiceImpl() {
        }

        virtual void Echo(RpcController* controller,
                          const EchoRequest* request,
                          EchoResponse* response,
                          Closure* done) {
            printf("service recv msg is %s\n", request->request().c_str());
            string send_str = "sevice echo is : " + request->request();
            response->set_response(send_str);
            printf("current tid is %u\n", pthread_self());
            if (done) {
                done->Run();
            }
        }
};


int main() {

    RpcServer& rpc_server = RpcServer::GetInstance();
    EchoServiceImpl service;
    rpc_server.RegisteService(&service);
    printf("Start the rpc sevice....\n");
    rpc_server.Start("127.0.0.1", "9999", 10);

    printf("Start wait.........\n");
    rpc_server.Wait();

    return 0;
}







/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
