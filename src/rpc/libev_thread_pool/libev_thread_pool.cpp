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

#include "thread_pool.h"

#include "pthread_mutex.h"

namespace libevrpc {

volatile ATOMIC_BOOL LibevThreadPool::running_(true);


LibevThreadPool::LibevThreadPool() :
   nthread_num_(10),
   max_nthread_num_(50) {
       Initialize();
}


LibevThreadPool::LibevThreadPool(int nthread) : 
    nthread_num_(nthread),
    max_nthread_num_(50) {
        Initialize();
}

LibevThreadPool::~LibevThreadPool() {
    Destroy();
    free(thread_ids_);
    thread_ids_ = NULL;
}

bool LibevThreadPool::Initialize() {
    // init the task list
    thread_ids_ = (pthread_t *) malloc(nthread_num_ * sizeof(pthread_t));
    task_list_.task_head = (Task*) malloc(sizeof(Task));
    memset(thread_ids_, 0, sizeof(thread_ids_));

    task_list_.task_head->next = task_list_.task_head;
    task_list_.task_head->prev = task_list_.task_head;
    return true;
}

RQ_ITEM* LibevThreadPool::RQNew() {
    return NULL;
}

bool LibevThreadPool::RQPush(RQ* req_queue, RQ_ITEM* req_item) {
    return truel
}

bool LibevThreadPool::Start() {
    // start all threads in the pool
    for (int i = 0; i < nthread_num_; ++i) {
        pthread_create(&thread_ids_[i], NULL, WorkerThread, this);
    }
    return true;
}

bool LibevThreadPool::Wait() {
    sleep(8);
    if (NULL != thread_ids_) {
        for (int i = 0; i < nthread_num_; ++i) {
            if (thread_ids_[i] == 0) {
                continue;
            }
            pthread_join(thread_ids_[i], NULL);
        }
    }
}

bool LibevThreadPool::Destroy() {
    running_ = false;
    ConditionBroadCast(cond_);

    Wait();

    task_list_.task_head->prev->next = NULL;
    Task* head_ptr = NULL;
    while (task_list_.task_head != NULL) {
        head_ptr = task_list_.task_head->next;
        task_list_.task_head = task_list_.task_head->next;
        free(head_ptr);
        head_ptr = NULL;
    }

    return true;

}

bool LibevThreadPool::Processor(void *(*process) (void *arg), void *arg) {
   Task* task_ptr = (Task*)malloc(sizeof(Task)); 
   task_ptr->process = process;
   task_ptr->param = arg;
   task_ptr->next = NULL;

   {
       // insert the task
       MutexLockGuard lock(task_mutex_);
       task_ptr->next = task_list_.task_head->next;
       task_ptr->prev = task_list_.task_head;
       task_list_.task_head->next->prev = task_ptr;
       task_list_.task_head->next = task_ptr;
       ++task_list_.task_num;
   }

   ConditionSignal(cond_);
   return true;
}

bool LibevThreadPool::DispatchRpcCall(void *(*rpc_call) (void *arg), void *arg) {
    if (NULL == libev_threads_) {
        perror("Dispatch rpc call failed! libev_threads ptr is null.");
        return false;
    }
    RQ_ITEM* rq_item = RQNew();
    if (NULL == rq_item) {
        perror("Get request item failed!");
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
    if (!RQPush(cur_thread->request_queue, rq_item)) {
        return false
    }

    char buf[1] = {'c'};
    if (write(cur_thread->notify_send_fd, buf, 1) != 1) {
        perror("Write to thread notify pipe failed!");
    }
    return true;
}

void *LibevThreadPool::WorkerThread(void *arg) {
    LibevThreadPool* pool = reinterpret_cast<LibevThreadPool*>(arg);
    TaskList& task_list = pool->task_list_;
    while (running_) {
        Task* task_proc = NULL;
        {
            MutexLockGuard lock(pool->task_mutex_);
            while (task_list.task_head->next == task_list.task_head) {
                ConditionWait(pool->cond_, pool->task_mutex_);
                if (false == running_) {
                    return NULL;
                    //pthread_exit(0);
                }
            }
            task_proc = task_list.task_head->prev;
            // remove the task from task list
            task_proc->next->prev = task_proc->prev;
            task_proc->prev->next = task_list.task_head;
            --task_list.task_num;
            task_proc->prev = NULL;
            task_proc->next = NULL; 
        }

        (*(task_proc->process))(task_proc->param);
    }

    return NULL;
}


} // end of namespace










/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
