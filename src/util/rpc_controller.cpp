/***************************************************************************
 *
 * Copyright (c) 2015 aishuyu, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file rpc_controller.cpp
 * @author aishuyu(asy5178@163.com)
 * @date 2016/03/31 20:47:09
 * @brief
 *
 **/

#include "rpc_controller.h"

namespace libevrpc {

RpcController::RpcController() {
}

RpcController::~RpcController() {
}

void RpcController::Reset() {
}

bool RpcController::Failed() const {
    return true;
}

string RpcController::ErrorText() const {
}

void RpcController::StartCancel() {
}

void RpcController::SetFailed(const string& reason) {
}

bool RpcController::IsCanceled() const {
}

void RpcController::NotifyOnCancel(Closure* callback) {
}



}  // end of namespace libevrpc









/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
