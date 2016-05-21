/***************************************************************************
 *
 * Copyright (c) 2016 aishuyu, Inc. All Rights Reserved
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
#include <unordered_map>
#include <unordered_set>

#include "rpc_heartbeat_server.h"
#include "config_parser/config_parser.h"
#include "util/pthread_mutex.h"
#include "util/disallow_copy_and_assign.h"
#include "util/thread.h"

namespace libevrpc {


struct ConnectionTimer {
    time_t start_time;
    time_t expire_time;
    int32_t fd;
    std::string client_addr;

    ConnectionTimer* next;
};

typedef std::shared_ptr<ConnectionTimer> CT_PTR;

typedef std::map<std::string, CT_PTR> CT_MAP;
typedef std::unordered_map<std::string, CT_PTR> CT_HASH_MAP;
typedef std::shared_ptr<CT_MAP> CT_MAP_PTR;
typedef std::shared_ptr<CT_HASH_MAP> CT_HASH_MAP_PTR;

typedef std::vector<CT_MAP_PTR> CTM_VEC;
typedef std::shared_ptr<CTM_VEC> CTM_VEC_PTR;
typedef std::vector<CT_HASH_MAP_PTR> CTHM_VEC;
typedef std::shared_ptr<CTHM_VEC> CTHM_VEC_PTR;

typedef std::vector<Mutex> MUTEX_VEC;
typedef std::shared_ptr<MUTEX_VEC> MUTEX_VEC_PTR;

typedef std::vector<std::string> LIST;
typedef std::shared_ptr<LIST> LIST_PTR;
typedef std::vector<LIST_PTR> LIST_PTR_LIST;
typedef std::shared_ptr<LIST_PTR_LIST> LIST_PPTR;

typedef std::unordered_set<std::string> HASH_SET;
typedef std::shared_ptr<HASH_SET> HASH_SET_PTR;

const int32_t buckets_size = 60;

class ConnectionTimerManager : public Thread {
    public:
        ~ConnectionTimerManager();

        static ConnectionTimerManager& GetInstance(const char* config_file);
        /*
         * every thread call InitTimerBuf once best,
         * otherwise make the connection_buf heavy!
         */
        int32_t InitTimerBuf();
        /*
         * every connection has come, PLEASE call,
         * otherwise the hearbeat is nothing to work
         */
        int32_t InsertConnectionTimer(
                const std::string& ip_addr,
                int32_t fd,
                int32_t buf_index);

        void DeleteConnectionTimer(
                const std::string& ip_addr,
                int32_t fd,
                int32_t buf_index);

        bool InsertRefreshConnectionInfo(std::string& ip_addr);

        virtual void Run();

    private:
        ConnectionTimerManager(const char* config_file);

        std::string GenerateTimerKey(const std::string& ip_addr, int32_t fd);
        int32_t GenerateBucketNum(const std::string& ori_key);
        /*
         * background thread call
         */
        bool ConnectionBufCrawler();

        DISALLOW_COPY_AND_ASSIGN(ConnectionTimerManager);

    private:
        /*
         * in fact, is vector and thread no safe!
         * connection_buf_ptr :
         *       one thread, on buf element
         *       thread buf index -> hash map(connection key -> connection timer)
         */
        CTHM_VEC_PTR connection_buf_ptr_;
        /*
         * one step, one time to update and refresh one buckets
         */
        CT_MAP_PTR connection_pool_buckets_[buckets_size];
        /*
         * connection_del_list_ptr_ :
         *       one thread, one connection deleted list
         *       thread index -> deleted list
         */
        LIST_PPTR connection_del_list_ptr_;
        /*
         * contains all client address
         */
        HASH_SET_PTR refresh_client_list_ptr_;

        MUTEX_VEC_PTR connection_buf_mutex_ptr_;
        MUTEX_VEC_PTR connection_bucket_mutex_ptr_;
        MUTEX_VEC_PTR connection_dellist_mutex_ptr_;
        ConfigParser& config_parser_instance_;

        RpcHeartbeatServer* rpc_heartbeat_server_ptr_;


        /*
         * buf_index_ : user to get the init index in order that multiThreads
         * call InitTimerBuf and get its buf index in connection_buf
         */
        int32_t buf_index_;
        int32_t bucket_index_;
        int32_t refresh_interval_;

        volatile bool running_;
        Mutex connection_pool_mutex_;
        Mutex refresh_client_list_mutex_;

};

}  // end of namespace libevrpc


#endif // __CONNECTION_TIMER_MANAGER_H



/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
