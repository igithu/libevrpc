/***************************************************************************
 *
 * Copyright (c) 2015 aishuyu, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file rpc_client_controller.cpp
 * @author aishuyu(asy5178@163.com)
 * @date 2016/04/01 19:31:21
 * @brief
 *
 **/


#include "rpc_client_controller.h"

namespace libevrpc {

using std::string;
using google::protobuf::Closure;

RpcClientController::RpcClientController() : err_text_("") {
}

RpcClientController::~RpcClientController() {
}

void RpcClientController::Reset() {
    err_text_.clear();
}

bool RpcClientController::Failed() const {
    return !err_text_.empty();
}

string RpcClientController::ErrorText() const {
    return err_text_;
}

void RpcClientController::StartCancel() {
    /*
     * TODO
     */
}

void RpcClientController::SetFailed(const string& reason) {
    err_text_ = reason;
}

bool RpcClientController::IsCanceled() const {
    /*
     * TODO
     */
}

void RpcClientController::NotifyOnCancel(Closure* callback) {
    /*
     * TODO
     */
}


}  // end of namespace libevrpc












/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
