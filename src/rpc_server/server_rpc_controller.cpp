/***************************************************************************
 *
 * Copyright (c) 2015 aishuyu, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file server_rpc_controller.cpp
 * @author aishuyu(asy5178@163.com)
 * @date 2016/04/04 00:12:42
 * @brief
 *
 **/


#include "server_rpc_controller.h"


namespace libevrpc {

using std::string;
using google::protobuf::Closure;

ServerRpcController::ServerRpcController() : error_info_("") {
}

ServerRpcController::~ServerRpcController() {
}

void ServerRpcController::Reset() {
    error_info_.clear();
}

bool ServerRpcController::Failed() const {
    return !error_info_.empty();
}

string ServerRpcController::ErrorText() const {
    return error_info_;
}

void ServerRpcController::StartCancel() {
    /*
     * TODO
     */
}

void ServerRpcController::SetFailed(const string& reason) {
    error_info_ = reason;
}

bool ServerRpcController::IsCanceled() const {
    /*
     * TODO
     */
}

void ServerRpcController::NotifyOnCancel(Closure* callback) {
    /*
     * TODO
     */
}

}  // namespace of libevrpc








/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
