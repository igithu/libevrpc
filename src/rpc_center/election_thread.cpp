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

#include "rpc_center.h"


namespace libevrpc {

ElectionThread::ElectionThread() :
    election_q_(new ElectionQueue()),
    running_(true) {
    election_q_->head = NULL;
    election_q_->tail = NULL;
}

ElectionThread::~ElectionThread() {
    Destory();
}

void ElectionThread::Destory() {
    if (NULL != election_q_) {
        for (EL_ITEM* el_item = election_q_->head;
             el_item != election_q_->tail && el_item != NULL;
             el_item = el_item->next) {
            delete el_item;
        }
        delete election_q_;
        election_q_ = NULL;
    }
}

void ElectionThread::Run() {
    if (NULL == election_q_) {
        election_q_ = new ElectionQueue();
    }
    /*
    if (!g_rpc_center.ProposalLeaderElection()) {
        fprintf(stderr, "Run the Election failed!\n");
        return;
    }
    */

    while (running_) {
        ElectionItem* el_item = PopElectionMessage();;
        if (NULL == el_item) {
            sleep(30);
            continue;
        }
        g_rpc_center.FastLeaderElection(el_item->centers_proto);

        delete el_item;
        sleep(30);
    }
    Destory();
    g_rpc_center.SetFastLeaderRunning(false);
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
