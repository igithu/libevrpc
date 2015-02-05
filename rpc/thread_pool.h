/***************************************************************************
 * 
 * Copyright (c) 2014 aishuyu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file thread_pool.h
 * @author aishuyu(asy5178@163.com)
 * @date 2014/11/07 15:32:07
 * @brief 
 *  
 **/




#ifndef  __THREAD_POOL_H_
#define  __THREAD_POOL_H_


//#include <pthread.h>
#include <errno.h>
#include <stdio.h>

#include "atomic.h"
#include "pthread_cond.h"

namespace libevrpc {

using namespace PUBLIC_UTIL;

struct Task {
    // callback funtion
    void *(*process)(void * arg);
    void *param;
    struct Task *prev;
    struct Task *next;
};

struct TaskList {
    int32_t task_num;
    struct Task *task_head;
};


class ThreadPool {
    public:
        // default ctor
        ThreadPool();

        ThreadPool(int nthread);

        virtual ~ThreadPool();

        bool Start();

        bool Wait();

        // nonblock call the processor and return shortly
        bool Processor(void *(*process) (void *arg), void *arg);

    private:
        bool Initialize();

        static void *WorkerThread(void *arg);

        bool Destroy();

    private:
        pthread_t *thread_ids_;

        int32_t nthread_num_;
        int32_t max_nthread_num_;
        static volatile ATOMIC_BOOL running_;

        PUBLIC_UTIL::Mutex task_mutex_;
        Condition cond_;
        TaskList task_list_;
};


} // end of namespace libevrpc

#endif  //__THREAD_POOL_H_

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
