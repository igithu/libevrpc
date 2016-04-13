/***************************************************************************
 *
 * Copyright (c) 2016 aishuyu, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file connection_timer_manager.cpp
 * @author aishuyu(asy5178@163.com)
 * @date 2016/03/08 23:46:33
 * @brief
 *
 **/


#include "connection_timer_manager.h"

#include <time.h>

#include "rpc_util.h"


namespace libevrpc {

using std::string;


ConnectionTimerManager::ConnectionTimerManager() :
    connection_buf_ptr_(new CTM_VEC()),
    connection_buf_mutex_ptr_(new MUTEX_VEC()),
    buf_index_(0),
    bucket_index_(0),
    refresh_interval_(30),
    running_(false) {
    for (int32_t i = 0; i < 60; ++i) {
        connection_pool_buckets_[i] = NULL
    }
}

ConnectionTimerManager::~ConnectionTimerManager() {
}

ConnectionTimerManager& ConnectionTimerManager::GetInstance() {
    static ConnectionTimerManager ctm_instance;
    return ctm_instance;
}

int32_t ConnectionTimerManager::InitTimerBuf() {
     CT_MAP_PTR ct_map_ptr(new CT_MAP());
     int32_t cur_index = 0;
     {
         MutexLockGuard lock(connection_pool_mutex_);
         connection_buf_ptr_->push_back(ct_map_ptr);
         connection_buf_mutex_ptr_->push_back(new Mutex());
         cur_index = buf_index_;
         ++buf_index_;
     }
     running_ = true;
     return cur_index;
}

int32_t ConnectionTimerManager::InsertConnectionTimer(
        const string& ip_addr,
        int32_t fd,
        int32_t buf_index) {
    CT_PTR ct_ptr(new ConnectionTimer());
    ct_ptr->fd = fd;
    ct_ptr->client_addr = std::move(ip_addr);
    ct_ptr->expire_time = time(NULL) + refresh_interval_;

    /*
     * every thread has its own buf_index
     * mutex is shared with background thread
     */
    Mutex& mutex = connection_buf_mutex_ptr_->at(buf_index);
    {
        MutexLockGuard guard(mutex)
        CT_MAP_PTR ctm_ptr = connection_buf_ptr_->at(buf_index);
        if (NULL == ctm_ptr) {
            /*
             * means the connection map is taken to in the connection
             * time wheel bucket
             */
            ctm_ptr = new CT_MAP();
        }
        /*
         * use auto in c++ 11
         */
        CT_MAP::iterator iter = ctm_ptr->find(t_key);
        if (iter == ctm_ptr->end()) {
            ctm_ptr->insert(std::make_pair(t_key, ct_ptr));
        } else {
            iter->sencond = ct_ptr;
        }
    }
    return t_key;
}

bool DeleteConnectionTimer(int32_t buf_index, int32_t connection_id) {
    CT_MAP_PTR ctm_ptr = connection_buf_ptr_->at(buf_index);
    if (NULL == ctm_ptr) {
        return false;
    }
    CT_MAP::iterator iter = ct_ptr->find(connection_id);
    if (iter == ct_ptr->end()) {
        /*
         * not exist
         */
        return true;
    }
    ct_ptr->erase(iter);
    return true;
}

void ConnectionTimerManager::Run() {
    bucket_index_ = 0;
    while (running_) {
        CT_MAP_PTR ctm_ptr = connection_pool_buckets_[bucket_index_];
        if (NULL == ctm_ptr || ctm_ptr->empty()) {
            sleep(3);
        }
        for (CT_MAP::iterator iter = ctm_ptr->begin(); iter != ctm_ptr->end(); ++iter) {
            CT_PTR ct_ptr = iter->second;
            if (NULL == ct_ptr) {
                continue;
            }
            if (ct_ptr->expire_time > time(NULL)) {
                // TODO  if client is gone, disconnect it! and remove it
                //       if client is fine, expire_time += refresh_interval_
            } else {
                break;
            }
        }
        bucket_index_ = (bucket_index_ + 1) % buckets_size;
        sleep(1);
    }
}

int32_t ConnectionTimerManager::GenerateTimerKey(const std::string& ip_addr, int32_t fd) {
    int32_t hash_code = BKDRHash(ip_addr);
    return hash_code + fd;
}

bool ConnectionTimerManager::ConnectionBufCrawler() {
    if (NULL == connection_buf_ptr_) {
        return false;
    }

    for (int32_t buf_index = 0; buf_index < connection_buf_ptr_->size(); ++buf_index) {
        CT_MAP_PTR local_ctm_ptr = NULL;
        {
            MutexLockGuard guard(connection_buf_mutex_ptr_->at(buf_index));
            CT_MAP_PTR& ctm_ptr = connection_buf_ptr_->at(buf_index);
            local_ctm_ptr = ctm_ptr;
            ctm_ptr = NULL;
        }
        if (NULL == local_ctm_ptr) {
            continue;
        }
        /*
         * Hash all connection into buckets
         */
        for (CT_MAP::iterator iter = local_ctm_ptr->begin();
             iter != local_ctm_ptr->end();
             ++iter) {
        }
    }
    return true;
}




}  // end of namespace libevrpc




/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
