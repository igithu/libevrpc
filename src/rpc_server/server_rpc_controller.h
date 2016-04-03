/***************************************************************************
 *
 * Copyright (c) 2015 aishuyu, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file server_rpc_controller.h
 * @author aishuyu(asy5178@163.com)
 * @date 2016/04/04 00:11:53
 * @brief
 *
 **/




#ifndef __SERVER_RPC_CONTROLLER_H
#define __SERVER_RPC_CONTROLLER_H


#include <google/protobuf/service.h>

namespace libevrpc {

class ServerRpcController : public google::protobuf::RpcController {
    public:
        ServerRpcController();
        ~ServerRpcController();

        /*
         *
         */
        virtual void Reset();
        /*
         *
         */
        virtual bool Failed() const;
        /*
         *
         */
        virtual std::string ErrorText() const;
        /*
         *
         */
        virtual void StartCancel();
        /*
         *
         */
        virtual void SetFailed(const std::string& reason);
        /*
         *
         */
        virtual bool IsCanceled() const;
        /*
         *
         */
        virtual void NotifyOnCancel(google::protobuf::Closure* callback);

    private:
        std::string error_info_;

};

}  // end of namespace libevrpc







#endif // __SERVER_RPC_CONTROLLER_H



/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
