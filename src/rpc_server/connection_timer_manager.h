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




#ifndef __CONNECTION_TIMER_H
#define __CONNECTION_TIMER_H

#include <vector>
#include <map>
#include <memory>
#include <string>

#include "disallow_copy_and_assign.h"
#include "thread.h"

namespace libevrpc {

typedef std::shared_ptr<ConnectionTimer> CI_PTR;
typedef std::map<int32_t, CI_PTR> CI_MAP;
typedef std::shared_ptr<CI_MAP> CI_MAP_PTR;
typedef std::vector<CI_MAP_PTR> CIM_VEC;
typedef std::shared_ptr<CIM_VEC> CIM_VEC_PTR;

struct ConnectionTimer {
    time_t start_time;
    time_t expire_time;
    int32_t fd;
    std::string client_addr;

    CI_PTR next;
};

class ConnectionTimerManager : public PUBLIC_UTIL::Thread {
    public:
        ~ConnectionTimerManager();

        static ConnectionTimerManager& GetInstance();
        int32_t InitTimerPool();
        int32_t InsertConnectionTimer(
                const std::string& ip_addr,
                int32_t fd,
                int32_t pool_index);

        bool DeleteConnectionTimer(
                int32_t pool_index,
                int32_t connection_id);

        virtual void Run();

    private:
        ConnectionTimerManager();

        int32_t GenrateTimerKey(const std::string& ip_addr, int32_t fd);

        DISALLOW_COPY_AND_ASSIGN(ConnectionTimerManager);

    private:
        /*
         * in fact, is vector and thread no safe!
         * NO push_back! except in InitTimerPool!
         */
        CIM_VEC_PTR connection_pool_ptr_;
        int32_t pool_index_;
        int32_t refresh_interval_;

        volatile bool running_;
        PUBLIC_UTIL::Mutex connection_pool_mutex_;

};

}  // end of namespace libevrpc


#endif // __CONNECTION_TIMER_H



/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
