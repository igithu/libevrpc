/***************************************************************************A
 * 
 * Copyright (c) 2014 aishuyu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/



/**
 * @file libev_thread_pool.h
 * @author aishuyu(asy5178@163.com)
 * @date 2014/11/07 15:32:07
 * @brief
 *
 **/




#ifndef  __LIBEV_THREAD_POOL_H_
#define  __LIBEV_THREAD_POOL_H_


#include <errno.h>
#include <stdio.h>
#include <stdint.h>

#include <ev.h>

#include "util/pthread_mutex.h"

namespace libevrpc {

/*
 * rpc call info item
 */
typedef struct RequestQueueItem RQ_ITEM;
struct RequestQueueItem {
    // callback funtion
    void *(*processor)(void * arg);
    void *param;
    RQ_ITEM *prev;
    RQ_ITEM *next;
    int32_t connection_id;
};

/*
 * rpc call queue
 */
class RequestQueue;
typedef RequestQueue RQ;
class RequestQueue {
    public:
        RQ_ITEM* head;
        RQ_ITEM* tail;
        Mutex q_mutex;
};

/*
 * thread with libev info
 */
class LibevThreadPool;
typedef struct {
    pthread_t thread_id;
    struct ev_loop* epoller;
    struct ev_io libev_watcher;
    int32_t notify_receive_fd;
    int32_t notify_send_fd;
    int32_t pool_index;
    long running_version;
    RQ* new_request_queue;
    LibevThreadPool* lt_pool;

    Mutex version_mutex;
} LIBEV_THREAD;

/*
 * use to retrieve data when the thread to be terminated
 */
struct RetrieveData {
    LibevThreadPool* retrieve_ltp;
    RQ_ITEM* retrieve_item;
};

class LibevThreadPool {
    public:
        LibevThreadPool();

        virtual ~LibevThreadPool();

        bool Start(int32_t num_threads = 10);
        bool Wait();

        /*
         * kill the current thread and restart a new thread to
         * replace it
         */
        bool RestartThread(pthread_t thread_id, long running_version);

        /*
         * nonblock call the processor and return shortly
         */
        bool DispatchRpcCall(void *(*process) (void *arg), void *arg);

    private:
        bool LibevThreadInitialization(int32_t num_threads);

        /*
         * request itesm op
         */
        RQ_ITEM* RQItemNew();
        RQ_ITEM* RQItemPop(RQ* req_queue);
        bool RQItemPush(RQ* req_queue, RQ_ITEM* req_item);
        bool RQItemFree(RQ_ITEM* req_item);

        /*
         * static fuction
         */
        static void LibevProcessor(struct ev_loop *loop, struct ev_io *watcher, int revents);
        static void *LibevWorker(void *arg);
        /*
         * where the thread exit exceedingly, do retrieve the data!
         */
        static void RetrieveDataAction(void *arg);

        bool Destroy();

    private:
        /*
         * threads
         */
        int32_t current_thread_;
        int32_t num_threads_;

        LIBEV_THREAD* libev_threads_;
        RQ_ITEM*  rqi_freelist_;

        Mutex rqi_freelist_mutex_;

        static int32_t item_per_alloc_;
        //static atomic_bool running_;
};


} // end of namespace libevrpc

#endif  //__LIBEV_THREAD_POOL_H_

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
