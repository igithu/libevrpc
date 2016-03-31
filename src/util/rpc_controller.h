/***************************************************************************
 *
 * Copyright (c) 2015 aishuyu, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file rpc_controller.h
 * @author aishuyu(asy5178@163.com)
 * @date 2016/03/31 20:41:10
 * @brief
 *
 **/




#ifndef __RPC_CONTROLLER_H
#define __RPC_CONTROLLER_H

#include <google/protobuf/service.h>


namespace libevrpc {

class RpcController : public google::protobuf::RpcController {
    public:
        RpcController();
        ~RpcController();

        void Reset();
        bool Failed() const;
        string ErrorText() const;
        void StartCancel();
        void SetFailed(const string& reason);
        bool IsCanceled() const;
        void NotifyOnCancel(Closure* callback);

};


}  // end of namespace libevrpc





#endif // __RPC_CONTROLLER_H



/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
