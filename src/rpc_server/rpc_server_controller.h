/***************************************************************************
 *
 * Copyright (c) 2015 aishuyu, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file rpc_server_controller.h
 * @author aishuyu(asy5178@163.com)
 * @date 2016/04/01 19:41:39
 * @brief
 *
 **/




#ifndef __RPC_SERVER_CONTROLLER_H
#define __RPC_SERVER_CONTROLLER_H


#include <google/protobuf/service.h>


namespace libevrpc {

class RpcServerController : public google::protobuf::RpcController {
    public:
        RpcServerController();
        ~RpcServerController();

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
        std::string err_text_;

};

}  // end of namespace libevrpc






#endif // __RPC_SERVER_CONTROLLER_H



/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
