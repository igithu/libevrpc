/***************************************************************************
 *
 * Copyright (c) 2016 aishuyu, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file client_rpc_controller.h
 * @author aishuyu(asy5178@163.com)
 * @date 2016/04/04 00:02:36
 * @brief
 *
 **/




#ifndef __CLIENT_RPC_CONTROLLER_H
#define __CLIENT_RPC_CONTROLLER_H

#include <google/protobuf/service.h>

#include "util/pthread_rwlock.h"

namespace libevrpc {

class ClientRpcController : public google::protobuf::RpcController {
    public:
        ClientRpcController();
        ~ClientRpcController();

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
        mutable RWLock error_info_rwlock_;

};

}  // end of namespace libevrpc







#endif // __CLIENT_RPC_CONTROLLER_H



/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
