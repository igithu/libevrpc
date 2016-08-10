/***************************************************************************
 *
 * Copyright (c) 2016 aishuyu, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file client_center_communicator.cpp
 * @author aishuyu(asy5178@163.com)
 * @date 2016/08/10 20:44:43
 * @brief
 *
 **/

#include "client_center_communicator.h"


namespace libevrpc {

ClientCenterCommunicator::ClientCenterCommunicator() {
}

ClientCenterCommunicator::~ClientCenterCommunicator() {
}

bool ClientCenterCommunicator::CreateCenterCommunication() {
    return true;
}

void ClientCenterCommunicator::Run() {
    /*
     * TODO：从Center服务集群更新数据
     */
}

}  // end of namespace libevrpc







/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
