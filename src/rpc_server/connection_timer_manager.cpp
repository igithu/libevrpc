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
    connection_buf_ptr_(new CTHM_VEC()),
    connection_del_list_ptr_(new INT_LIST_PTR_LIST()),
    connection_buf_mutex_ptr_(new MUTEX_VEC()),
    connection_bucket_mutex_ptr_(new MUTEX_VEC()),
    connection_dellist_mutex_ptr_(new MUTEX_VEC()),
    buf_index_(0),
    bucket_index_(0),
    refresh_interval_(30),
    running_(false) {
    for (int32_t i = 0; i < 60; ++i) {
        connection_pool_buckets_[i] = NULL;
    }

    for (int32_t i = 0; i < buckets_size; ++i) {
        //connection_buf_mutex_ptr_-
    }
}

ConnectionTimerManager::~ConnectionTimerManager() {
}

ConnectionTimerManager& ConnectionTimerManager::GetInstance() {
    static ConnectionTimerManager ctm_instance;
    return ctm_instance;
}

int32_t ConnectionTimerManager::InitTimerBuf() {
     CT_HASH_MAP_PTR ctm_ptr(new CT_HASH_MAP());
     int32_t cur_buf_index = 0;
     {
         MutexLockGuard lock(connection_pool_mutex_);
         connection_buf_ptr_->push_back(ctm_ptr);
         INT_LIST_PTR ilp(new INT_LIST());
         connection_del_list_ptr_->push_back(ilp);

         Mutex buf_mutex;
         connection_buf_mutex_ptr_->push_back(std::move(buf_mutex));
         Mutex bucket_mutex;
         connection_bucket_mutex_ptr_->push_back(std::move(bucket_mutex));
         Mutex dellist_mutex;
         connection_dellist_mutex_ptr_->push_back(std::move(dellist_mutex));
         cur_buf_index = buf_index_;
         ++buf_index_;
     }
     running_ = true;
     return cur_buf_index;
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
        CT_HASH_MAP_PTR& ctm_ptr = connection_buf_ptr_->at(buf_index);
        if (NULL == ctm_ptr) {
            /*
             * means the connection map is taken to in the connection
             * time wheel bucket
             */
            ctm_ptr.reset(new CT_HASH_MAP());
        }
        int32_t ct_key = GenerateTimerKey(ip_addr, fd);
        ctm_ptr->insert(std::make_pair(ct_key, ct_ptr));
    }
    return 0;
}

void ConnectionTimerManager::DeleteConnectionTimer(
        const std::string& ip_addr,
        int32_t fd,
        int32_t buf_index) {
    int32_t ct_key = GenerateTimerKey(ip_addr, fd);
    Mutex& mutex = connection_dellist_mutex_ptr_->at(buf_index);
    {
        MutexLockGuard guard(mutex);
        INT_LIST_PTR& ilp = connection_del_list_ptr_->at(buf_index);
        if (NULL == ilp) {
            ilp->reset(new INT_LIST());
        }
        ilp->push_back(ct_key);
    }
}

void ConnectionTimerManager::Run() {
    bucket_index_ = 0;
    while (running_) {
        ConnectionBufCrawler();
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
        CT_HASH_MAP_PTR local_ctm_ptr = NULL;
        {
            MutexLockGuard guard(connection_buf_mutex_ptr_->at(buf_index));
            CT_HASH_MAP_PTR& ctm_ptr =  connection_buf_ptr_->at(buf_index);
            local_ctm_ptr = ctm_ptr;
            ctm_ptr = NULL;
        }
        if (NULL == local_ctm_ptr) {
            continue;
        }
        /*
         * Hash(by modulo for now) all connection into connection_pool buckets
         */
        for (CT_HASH_MAP::iterator iter = local_ctm_ptr->begin();
             iter != local_ctm_ptr->end();
             ++iter) {
            int32_t c_key = iter->first;
            CT_PTR& ct_ptr = iter->second;
            int32_t bucket_num = c_key % buckets_size;
            Mutex& bucket_mutex = connection_bucket_mutex_ptr_->at(bucket_num);
            {
                MutexLockGuard guard(bucket_mutex);
                CT_MAP_PTR& ct_map_ptr = connection_pool_buckets_[bucket_num];
                if (NULL == ct_map_ptr) {
                    ct_map_ptr.reset(new CT_MAP());
                }
                ct_map_ptr->insert(std::make_pair(c_key, ct_ptr));
            }
        }
    }
    return true;
}




}  // end of namespace libevrpc




/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
