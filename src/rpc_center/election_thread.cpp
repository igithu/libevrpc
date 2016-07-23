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

ElectionThread::ElectionThread() : election_q_(new ElectionQueue()){
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
}

bool ElectionThread::PushElectionMessage(const string& election_msg) {
    return true;
}



}  // end of namespace libevrpc






/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
