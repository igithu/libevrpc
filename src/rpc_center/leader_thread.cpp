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

#include "rpc_center.h"


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
             fw_item != follower_q_->tail && fw_item != NULL;
             fw_item = fw_item->next) {
            delete fw_item;
        }
        follower_q_ = NULL;
    }
}

void LeaderThread::Run() {
    if (NULL == follower_q_) {
        follower_q_ = new FollowerQueue();
    }
    while (running_) {
        FollowerItem* fw_item = PopFollowerMessage();
        if (NULL == fw_item) {
            sleep(20);
            continue;
        }
        RpcCenter::GetInstance(g_config_file).ProcessCenterData(fw_item->conn_fd, fw_item->centers_proto);
        close(fw_item->conn_fd);
        delete fw_item;
    }
    Destory();
}

void LeaderThread::StopThread() {
    running_ = false;
}

bool LeaderThread::PushFollowerMessage(int32_t fd, const CentersProto& centers_proto) {
    FollowerItem* fi = new FollowerItem();
    fi->centers_proto.CopyFrom(centers_proto);
    fi->conn_fd = fd;
    fi->next = NULL;
    {
        MutexLockGuard lock(fq_mutex_);
        if (NULL == follower_q_->tail) {
            follower_q_->head = fi;
        } else {
            follower_q_->tail->next = fi;
        }
        follower_q_->tail = fi;
    }
    return true;
}

FollowerItem* LeaderThread::PopFollowerMessage() {
    FollowerItem* fw_item = NULL;
    {
        MutexLockGuard lock(fq_mutex_);
        fw_item = follower_q_->head;
        if (NULL != fw_item) {
            follower_q_->head = fw_item->next;
            if (NULL == follower_q_->head) {
                follower_q_->tail = NULL;
            }
        }
    }
    return fw_item;
}



}  // end of namespace libevrpc











/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
