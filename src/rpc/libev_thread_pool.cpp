/***************************************************************************
 * 
 * Copyright (c) 2014 aishuyu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file thread_pool.cpp
 * @author aishuyu(asy5178@163.com)
 * @date 2014/11/07 18:44:20
 * @brief 
 *  
 **/

#include "libev_thread_pool.h"

#include <stdlib.h>

namespace libevrpc {

using namespace PUBLIC_UTIL;

int32_t LibevThreadPool::item_per_alloc_ = 64;
// atomic_bool LibevThreadPool::running_ = true;

LibevThreadPool::LibevThreadPool() :
    current_thread_(-1),
    num_threads_(10),
    libev_threads_(NULL),
    rqi_freelist_(NULL) {
}

LibevThreadPool::~LibevThreadPool() {
    Destroy();
}

bool LibevThreadPool::LibevThreadInitialization(int num_threads) {
    num_threads_ = num_threads;
    libev_threads_ = (LIBEV_THREAD*)calloc(num_threads_, sizeof(LIBEV_THREAD));

    rqi_freelist_ = NULL;

    if (NULL == libev_threads_) {
        return false;
    }

    for (int32_t i = 0; i < num_threads_; ++i) {
        int32_t fds[2];
        if (pipe(fds)) {
            perror("Create notify pipe failed!");
            exit(1);
        }
        /*
         * start to init the libev info in every thread
         */
        LIBEV_THREAD* cur_thread = &libev_threads_[i];
        cur_thread->notify_receive_fd = fds[0];
        cur_thread->notify_send_fd = fds[1];

        cur_thread->epoller = ev_loop_new(0);
        if (NULL == cur_thread->epoller) {
            perror("Ev loop new failed!");
            exit(-1);
        }
        cur_thread->lt_pool = this;
        ev_io_init(
                &cur_thread->libev_watcher,
                LibevThreadPool::LibevProcessor,
                cur_thread->notify_receive_fd,
                EV_READ);
        ev_io_start(cur_thread->epoller, &cur_thread->libev_watcher);

        cur_thread->new_request_queue = (RQ*)malloc(sizeof(RQ));
        if (NULL == cur_thread->new_request_queue) {
            perror("Failed to allocate memory for request queue");
            exit(-1);
        }
        {
            MutexLockGuard lock(cur_thread->new_request_queue->q_mutex);
            cur_thread->new_request_queue->head = NULL;
            cur_thread->new_request_queue->tail = NULL;
        }
    }

    for (int i = 0; i < num_threads_; ++i) {
        LIBEV_THREAD* lt = libev_threads_ + i;
        pthread_create(&(lt->thread_id), NULL, LibevThreadPool::LibevWorker, lt);
    }

    return true;
}

void *LibevThreadPool::LibevWorker(void *arg) {
    LIBEV_THREAD* me = (LIBEV_THREAD*)arg;

    ev_run(me->epoller, 0);
//    while(running_) {
//        ev_loop(me->epoller, 0);
//    }
}

RQ_ITEM* LibevThreadPool::RQItemNew() {
    RQ_ITEM *rq_item = NULL;
    {
        MutexLockGuard lock(rqi_freelist_mutex_);
        if (NULL != rqi_freelist_) {
            rq_item = rqi_freelist_;
            rqi_freelist_ = rq_item->next;
        }
    }

    if (NULL == rq_item) {
        rq_item = (RQ_ITEM*)malloc(sizeof(RQ_ITEM) * item_per_alloc_);
        if (NULL == rq_item) {
            perror("Alloc the item mem failed!");
            return NULL;
        }
        for (int i = 0; i < item_per_alloc_; ++i) {
            rq_item[i - 1].next = &rq_item[i];
        }
        {
            MutexLockGuard lock(rqi_freelist_mutex_);
            rq_item[item_per_alloc_ - 1].next = rqi_freelist_;
            rqi_freelist_ = &rq_item[1];
        }
    }
    return NULL;
}

RQ_ITEM* LibevThreadPool::RQItemPop(RQ* req_queue) {
    RQ_ITEM* rq_item = NULL;
    {
        MutexLockGuard lock(req_queue->q_mutex);
        rq_item = req_queue->head;
        if (NULL != rq_item) {
            req_queue->head = rq_item->next;
            if (NULL == req_queue->head) {
                req_queue->tail = NULL;
            }
        }
    }
    return rq_item;
}

bool LibevThreadPool::RQItemPush(RQ* req_queue, RQ_ITEM* rq_item) {
    rq_item->next = NULL;
    {
        MutexLockGuard lock(req_queue->q_mutex);
        if (NULL == req_queue->tail) {
            req_queue->head = rq_item;
        } else {
            req_queue->tail->next = rq_item;
        }
        req_queue->tail = rq_item;
    }
    return true;
}

bool LibevThreadPool::RQItemFree(RQ_ITEM* rq_item) {
    MutexLockGuard lock(rqi_freelist_mutex_);
    rq_item->next = rqi_freelist_;
    rqi_freelist_ = rq_item;
    return true;
}

bool LibevThreadPool::Start() {
    LibevThreadInitialization(20);
    return true;
}

bool LibevThreadPool::Wait() {
    for (int32_t i = 0; i < num_threads_; ++i) {
        pthread_t tid = (libev_threads_ + i)->thread_id;
        pthread_join(tid, 0);
    }
}

bool LibevThreadPool::Destroy() {
    for (int32_t i = 0; i < num_threads_; ++i) {
        LIBEV_THREAD* lt = libev_threads_ + i;
        ev_io_stop(lt->epoller, &(lt->libev_watcher));
        ev_break (lt->epoller, EVBREAK_ALL);
        ev_loop_destroy(lt->epoller);
    }
    Wait();
    return true;

}

bool LibevThreadPool::DispatchRpcCall(void *(*rpc_call) (void *arg), void *arg) {
    if (NULL == libev_threads_) {
        perror("Dispatch rpc call failed! libev_threads ptr is null.");
        return false;
    }

    RQ_ITEM* rq_item = RQItemNew();
    if (NULL == rq_item) {
        return false;
    }

    int32_t cur_tid = (current_thread_ + 1) % num_threads_;
    LIBEV_THREAD* cur_thread = libev_threads_ + cur_tid;
    current_thread_ = cur_tid;

    /*
     * set req item data
     */
    rq_item->processor = rpc_call;
    rq_item->param = arg;
    if (!RQItemPush(cur_thread->new_request_queue, rq_item)) {
        return false;
    }

    char buf[1] = {'c'};
    if (write(cur_thread->notify_send_fd, buf, 1) != 1) {
        perror("Write to thread notify pipe failed!");
    }
    return true;
}

void LibevThreadPool::LibevProcessor(struct ev_loop *loop, struct ev_io *watcher, int revents) {
    LIBEV_THREAD *me = (LIBEV_THREAD*)(watcher->data);
    if (NULL == me) {
        perror("LIBEV_THREAD data is NULL!");
        return;
    }

    char buf[1];
    if (read(watcher->fd, buf, 1) != 1) {
        perror("Can't read from libevent pipe!");
    }
    LibevThreadPool* lt_pool = me->lt_pool;
    switch (buf[0]) {
        case 'c':
            RQ_ITEM* item = lt_pool->RQItemPop(me->new_request_queue);
            (*(item->processor))(item->param);
    }
}


} // end of namespace










/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
