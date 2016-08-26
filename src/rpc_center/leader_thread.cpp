/***************************************************************************
 *
 * Copyright (c) 2016 aishuyu, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file leader_thread.cpp
 * @author aishuyu(asy5178@163.com)
 * @date 2016/08/26 20:17:50
 * @brief
 *
 **/



#include "leader_thread.h"


namespace libevrpc {


LeaderThread::LeaderThread():
    follower_q_(new FollowerQueue()),
    running_(false) {
    follower_q_->head = NULL;
    follower_q_->tail = NULL;
}

LeaderThread::~LeaderThread() {
    Destory();
}

void LeaderThread::Destory() {
    if (NULL != follower_q_) {
        for (FW_ITEM* fw_item = follower_q_->head;
             fw_item != fw_item->tail && fw_item != NULL;
             fw_item = fw_item->next) {
            delete el_item;
        }
        delete fw_item;
        follower_q_ = NULL;
    }
}

void LeaderThread::Run() {
}

void LeaderThread::StopThread() {
}

bool LeaderThread::PushFollowerMessage(int32_t fd, const CentersProto& centers_proto) {
    return true;
}

FollowerItem* LeaderThread::PopFollowerMessage() {
}



}  // end of namespace libevrpc











/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
