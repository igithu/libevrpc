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

#include "util/rpc_util.h"


namespace libevrpc {

using std::string;


ConnectionTimerManager::ConnectionTimerManager() :
    connection_buf_ptr_(new BUF_LIST()),
    connection_buf_mutex_ptr_(new MUTEX_VEC()),
    buf_index_(0),
    bucket_index_(0),
    refresh_interval_(30),
    running_(false) {
    for (int32_t i = 0; i < 60; ++i) {
        connection_pool_buckets_[i] = NULL;
    }
}

ConnectionTimerManager::~ConnectionTimerManager() {
}

ConnectionTimerManager& ConnectionTimerManager::GetInstance() {
    static ConnectionTimerManager ctm_instance;
    return ctm_instance;
}

int32_t ConnectionTimerManager::InitTimerBuf() {
     CTL_PTR ctl_ptr(new CT_PTR_LIST());
     int32_t cur_index = 0;
     {
         MutexLockGuard lock(connection_pool_mutex_);
         connection_buf_ptr_->push_back(ctl_ptr);
         // connection_buf_mutex_ptr_->push_back(new Mutex());
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
        MutexLockGuard guard(mutex);
        CTL_PTR ctl_ptr = connection_buf_ptr_->at(buf_index);
        if (NULL == ctl_ptr) {
            /*
             * means the connection map is taken to in the connection
             * time wheel bucket
             */
            ctl_ptr.reset(new CT_PTR_LIST());
        }
        ctl_ptr->push_back(ct_ptr);
    }
    return 0;
}

int32_t ConnectionTimerManager::GetBucketNum(const std::string& ip_addr, int32_t fd) {
    return GenerateTimerKey(ip_addr, fd) % buckets_size;
}

void ConnectionTimerManager::Run() {
    bucket_index_ = 0;
    while (running_) {
        CT_MAP_PTR& ctm_ptr = connection_pool_buckets_[bucket_index_];
        if (NULL == ctm_ptr || ctm_ptr->empty()) {
            sleep(3);
            continue;
        }
        for (CT_MAP::iterator iter = ctm_ptr->begin(); iter != ctm_ptr->end(); ++iter) {
            CT_PTR& ct_ptr = iter->second;
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
    uint32_t hash_code = BKDRHash(ip_addr.c_str());
    return hash_code + fd;
}

bool ConnectionTimerManager::ConnectionBufCrawler() {
    if (NULL == connection_buf_ptr_) {
        return false;
    }

    for (int32_t buf_index = 0; buf_index < connection_buf_ptr_->size(); ++buf_index) {
        CTL_PTR local_ctl_ptr = NULL;
        {
            MutexLockGuard guard(connection_buf_mutex_ptr_->at(buf_index));
            CTL_PTR& ctl_ptr = connection_buf_ptr_->at(buf_index);
            local_ctl_ptr = ctl_ptr;
            ctl_ptr = NULL;
        }
        if (NULL == local_ctl_ptr) {
            continue;
        }
        /*
         * Hash all connection into connection_pool buckets
         */
        for (CT_PTR_LIST::iterator iter = local_ctl_ptr->begin();
             iter != local_ctl_ptr->end();
             ++iter) {
            CT_PTR& ct_ptr = *iter;
            int32_t c_key = GenerateTimerKey(ct_ptr->client_addr, ct_ptr->fd);
            int32_t bucket_num = c_key % buckets_size;
            CT_MAP_PTR& ct_map_ptr = connection_pool_buckets_[bucket_num];
            if (NULL == ct_map_ptr) {
                ct_map_ptr.reset(new CT_MAP());
            }
            ct_map_ptr->insert(std::make_pair(c_key, ct_ptr));
        }
    }
    return true;
}




}  // end of namespace libevrpc




/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
