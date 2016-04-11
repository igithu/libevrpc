/***************************************************************************
 *
 * Copyright (c) 2016 aishuyu, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file client_rpc_controller.cpp
 * @author aishuyu(asy5178@163.com)
 * @date 2016/04/04 00:04:09
 * @brief
 *
 **/




#include "client_rpc_controller.h"

namespace libevrpc {

using std::string;
using google::protobuf::Closure;

ClientRpcController::ClientRpcController() : error_info_("") {
}

ClientRpcController::~ClientRpcController() {
}

void ClientRpcController::Reset() {
    error_info_.clear();
}

bool ClientRpcController::Failed() const {
    return !error_info_.empty();
}

string ClientRpcController::ErrorText() const {
    ReadLockGuard rguard(error_info_rwlock_);
    return error_info_;
}

void ClientRpcController::StartCancel() {
    /*
     * TODO
     */
}

void ClientRpcController::SetFailed(const string& reason) {
    WriteLockGuard wguard(error_info_rwlock_);
    error_info_.append(reason + "\n");
}

bool ClientRpcController::IsCanceled() const {
    /*
     * TODO
     */
}

void ClientRpcController::NotifyOnCancel(Closure* callback) {
    /*
     * TODO
     */
}

}  // namespace of libevrpc





/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
