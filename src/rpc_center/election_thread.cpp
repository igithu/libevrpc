/***************************************************************************
 *
 * Copyright (c) 2016 aishuyu, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file election_thread.cpp
 * @author aishuyu(asy5178@163.com)
 * @date 2016/07/22 16:54:22
 * @brief
 *
 **/


#include "election_thread.h"


namespace libevrpc {

ElectionThread::ElectionThread() :
    election_q_(new ElectionQueue()),
    running_(true) {
    election_q_->head = NULL;
    election_q_->tail = NULL;
}

ElectionThread::~ElectionThread() {
    if (NULL != election_q_) {
        for (EL_ITEM* el_item = election_q_->head;
             el_item != election_q_->tail && el_item != NULL;
             el_item = el_item->next) {
            delete el_item;
        }
        delete election_q_;
    }
}

void ElectionThread::Run() {
    while (running_) {
        ElectionItem* el_item = PopElectionMessage();;
        if (NULL == el_item) {
            sleep(5);
            continue;
        }
    }
}

void ElectionThread::StopThread() {
    running_ = false;
}

bool ElectionThread::PushElectionMessage(const string& election_msg) {
    ElectionItem* ei = new ElectionItem();
    ei->centers_proto.ParseFromString(election_msg);
    ei->next = NULL;
    {
        MutexLockGuard lock(eq_mutex_);
        if (NULL == election_q_->tail) {
            election_q_->head = ei;
        } else {
            election_q_->tail->next = ei;
        }
        election_q_->tail = ei;
    }
    return true;
}

ElectionItem* ElectionThread::PopElectionMessage() {
    ElectionItem* el_item = NULL;
    {
        MutexLockGuard lock(eq_mutex_);
        el_item = election_q_->head;
        if (NULL != el_item) {
            election_q_->head = el_item->next;
            if (NULL == election_q_->head) {
                election_q_->tail = NULL;
            }
        }
    }
    return el_item;
}




}  // end of namespace libevrpc






/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
