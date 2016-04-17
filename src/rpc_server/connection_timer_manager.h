/***************************************************************************
 *
 * Copyright (c) 2015 aishuyu, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file connection_timer_manager.h
 * @author aishuyu(asy5178@163.com)
 * @date 2016/03/08 23:39:34
 * @brief
 *
 **/




#ifndef __CONNECTION_TIMER_MANAGER_H
#define __CONNECTION_TIMER_MANAGER_H

#include <vector>
#include <map>
#include <memory>
#include <string>

#include "util/pthread_mutex.h"
#include "util/thread.h"
#include "util/disallow_copy_and_assign.h"

namespace libevrpc {


struct ConnectionTimer {
    time_t start_time;
    time_t expire_time;
    int32_t fd;
    std::string client_addr;

    ConnectionTimer* next;
};


typedef std::shared_ptr<ConnectionTimer> CT_PTR;
typedef std::vector<CT_PTR> CT_PTR_LIST;
typedef std::shared_ptr<CT_PTR_LIST> CTL_PTR;
typedef std::vector<CTL_PTR> BUF_LIST;
typedef std::shared_ptr<BUF_LIST> BUF_LIST_PTR;

typedef std::map<int32_t, CT_PTR> CT_MAP;
typedef std::shared_ptr<CT_MAP> CT_MAP_PTR;
typedef std::vector<CT_MAP_PTR> CTM_VEC;

typedef std::vector<Mutex> MUTEX_VEC;
typedef std::shared_ptr<MUTEX_VEC> MUTEX_VEC_PTR;

const int32_t buckets_size = 60;

class ConnectionTimerManager : public Thread {
    public:
        ~ConnectionTimerManager();

        static ConnectionTimerManager& GetInstance();
        int32_t InitTimerBuf();
        int32_t InsertConnectionTimer(
                const std::string& ip_addr,
                int32_t fd,
                int32_t buf_index);

        void DeleteConnectionTimer(
                const std::string& ip_addr,
                int32_t fd,
                int32_t buf_index);

        virtual void Run();

    private:
        ConnectionTimerManager();

        int32_t GenerateTimerKey(const std::string& ip_addr, int32_t fd);
        bool ConnectionBufCrawler();

        DISALLOW_COPY_AND_ASSIGN(ConnectionTimerManager);

    private:
        /*
         * in fact, is vector and thread no safe!
         * NO push_back! except in InitTimerPool!
         */
        BUF_LIST_PTR connection_buf_ptr_;
        CT_MAP_PTR connection_pool_buckets_[buckets_size];

        MUTEX_VEC_PTR connection_buf_mutex_ptr_;
        MUTEX_VEC_PTR connection_bucket_mutex_ptr_;
        /*
         * buf_index_ : user to get the init index in order that multiThreads
         * call InitTimerBuf and get its buf index in connection_buf
         */
        int32_t buf_index_;
        int32_t bucket_index_;
        int32_t refresh_interval_;

        volatile bool running_;
        Mutex connection_pool_mutex_;

};

}  // end of namespace libevrpc


#endif // __CONNECTION_TIMER_MANAGER_H



/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
