/***************************************************************************
 *
 * Copyright (c) 2015 aishuyu, Inc. All Rights Reserved
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
    connection_pool_ptr_(new CIM_VEC()),
    pool_index_(0),
    refresh_interval_(30),
    running_(false) {
}

ConnectionTimerManager::~ConnectionTimerManager() {
    connection_array_ptr_.reset();
}

ConnectionTimerManager& ConnectionTimerManager::GetInstance() {
    static ConnectionTimerManager ctm_instance;
    return ctm_instance;
}

int32_t ConnectionTimerManager::InitTimerPool() {
     CI_MAP_PTR ci_map_ptr(new CI_MAP());
     int32_t cur_index = 0;
     {
         MutexLockGuard lock(connection_pool_mutex_);
         connection_pool_ptr_->push_back(ci_map_ptr);
         cur_index = pool_index_;
         ++pool_index_;
     }
     running_ = true;
     return cur_index;
}

int32_t ConnectionTimerManager::InsertConnectionTimer(
        const string& ip_addr,
        int32_t fd,
        int32_t pool_index) {
    /*
     * every thread has its own pool_index
     */
    CI_MAP_PTR cim_ptr = connection_pool_ptr_->at(pool_index);
    if (NULL == cim_ptr) {
        return false;
    }
    int32_t t_key = GenrateTimerKey(ip_addr, fd);
    CI_PTR ci_ptr(new ConnectionTimer());
    ci_ptr->fd = fd;
    ci_ptr->client_addr = std::move(ip_addr);
    ci_ptr->expire_time = time(NULL) + refresh_interval_;

    /*
     * use auto in c++ 11
     */
    CI_MAP::iterator iter = cim_ptr->find(t_key);
    if (iter == cim_ptr->end()) {
        cim_ptr->insert(std::make_pair(t_key, ci_ptr));
    } else {
        iter->sencond = ci_ptr;
    }
    return t_key;
}

bool DeleteConnectionTimer(int32_t pool_index, int32_t connection_id) {
    CI_MAP_PTR cim_ptr = connection_pool_ptr_->at(pool_index);
    if (NULL == cim_ptr) {
        return false;
    }
    CI_MAP::iterator iter = ci_ptr->find(connection_id);
    if (iter == ci_ptr->end()) {
        /*
         * not exist
         */
        return true;
    }
    ci_ptr->erase(iter);
    return true;
}

void ConnectionTimerManager::Run() {
    pool_index_ = 0;
    int32_t total_size = connection_pool_ptr_->size();
    while (running_) {
        CI_MAP_PTR cim_ptr = connection_pool_ptr_->at(pool_index_);
        if (NULL == cim_ptr || cim_ptr->empty()) {
            sleep(3);
        }
        for (CI_MAP::iterator iter = cim_ptr->begin(); iter != cim_ptr->end(); ++iter) {
            CI_PTR ci_ptr = iter->second;
            if (NULL == ci_ptr) {
                continue;
            }
            if (ci_ptr->expire_time > time(NULL)) {
                // TODO  if client is gone, disconnect it! and remove it
                //       if clint is fine, expire_time += refresh_interval_
            } else {
                break;
            }
        }
        pool_index_ = (pool_index_ + 1) % total_size;
        sleep(1);
    }
}

int32_t GenrateTimerKey(const std::string& ip_addr, int32_t fd) {
    int32_t hash_code = BKDRHash(ip_addr);
    return hash_code + fd;
}



}  // end of namespace libevrpc




/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
