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


namespace libevrpc {

using std::string;

ConnectionTimerManager::ConnectionTimerManager() :
    connection_pool_ptr_(new CIM_VEC()),
    pool_index_(0) {
}

ConnectionTimerManager::~ConnectionTimerManager() {
    connection_array_ptr_.reset();
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
     return cur_index;
}

bool ConnectionTimerManager::InsertConnectinTimer(
        const std::string& ip_addr,
        int32_t fd,
        int32_t pool_index) {
    return true;
}

void ConnectionTimerManager::Run() {
}

}  // end of namespace libevrpc




/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
