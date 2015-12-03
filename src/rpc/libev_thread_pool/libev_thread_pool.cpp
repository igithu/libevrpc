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

volatile ATOMIC_BOOL ThreadPool::running_(true);


ThreadPool::ThreadPool() :
   nthread_num_(10),
   max_nthread_num_(50) {
       Initialize();
}


ThreadPool::ThreadPool(int nthread) : 
    nthread_num_(nthread),
    max_nthread_num_(50) {
        Initialize();
}

ThreadPool::~ThreadPool() {
    Destroy();
    free(thread_ids_);
    thread_ids_ = NULL;
}

bool ThreadPool::Initialize() {
    // init the task list
    thread_ids_ = (pthread_t *) malloc(nthread_num_ * sizeof(pthread_t));
    task_list_.task_head = (Task*) malloc(sizeof(Task));
    memset(thread_ids_, 0, sizeof(thread_ids_));

    task_list_.task_head->next = task_list_.task_head;
    task_list_.task_head->prev = task_list_.task_head;
    return true;
}


bool ThreadPool::Start() {
    // start all threads in the pool
    for (int i = 0; i < nthread_num_; ++i) {
        pthread_create(&thread_ids_[i], NULL, WorkerThread, this);
    }
    return true;
}

bool ThreadPool::Wait() {
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

bool ThreadPool::Destroy() {
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

bool ThreadPool::Processor(void *(*process) (void *arg), void *arg) {
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

void *ThreadPool::WorkerThread(void *arg) {
    ThreadPool* pool = reinterpret_cast<ThreadPool*>(arg);
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
