/***************************************************************************
 *
 * Copyright (c) 2015 aishuyu, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file rpc_server_controller.cpp
 * @author aishuyu(asy5178@163.com)
 * @date 2016/04/01 19:44:14
 * @brief
 *
 **/

#include "rpc_server_controller.h"


namespace libevrpc {

using std::string;
using google::protobuf::Closure;

RpcServerController::RpcServerController() : err_text_("") {
}

RpcServerController::~RpcServerController() {
}

void RpcServerController::Reset() {
    err_text_.clear();
}

bool RpcServerController::Failed() const {
    return !err_text_.empty();
}

string RpcServerController::ErrorText() const {
    return err_text_;
}

void RpcServerController::StartCancel() {
    /*
     * TODO
     */
}

void RpcServerController::SetFailed(const string& reason) {
    err_text_ = reason;
}

bool RpcServerController::IsCanceled() const {
    /*
     * TODO
     */
}

void RpcServerController::NotifyOnCancel(Closure* callback) {
    /*
     * TODO
     */
}


}  // end of namespace libevrpc












/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
