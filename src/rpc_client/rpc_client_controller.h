/***************************************************************************
 *
 * Copyright (c) 2015 aishuyu, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file rpc_client_controller.h
 * @author aishuyu(asy5178@163.com)
 * @date 2016/04/01 19:27:34
 * @brief
 *
 **/




#ifndef __RPC_CLIENT_CONTROLLER_H
#define __RPC_CLIENT_CONTROLLER_H

#include <google/protobuf/service.h>

#include "util/pthread_rwlock.h"

namespace libevrpc {

class RpcClientController : public google::protobuf::RpcController {
    public:
        RpcClientController();
        ~RpcClientController();

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




#endif // __RPC_CLIENT_CONTROLLER_H



/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
