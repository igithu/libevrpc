/***************************************************************************
 *
 * Copyright (c) 2014 aishuyu.com, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file thread.h
 * @author aishuyu(asy5178@163.com)
 * @date 2014/10/30 21:39:40
 * @brief
 *
 **/




#ifndef  __THREAD_H_
#define  __THREAD_H_

#include <pthread.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>

#include <atomic>

namespace libevrpc {

class Thread {
    public:
        Thread() : thread_running_(false) {
        }

        virtual ~Thread() {
            Stop();
            Wait();
        }

        bool Start() {
            if (true == thread_running_) {
                return true;
            }
            if (pthread_create(&tid_, NULL, hook, this) != 0) {
                return false;
            }
            thread_running_ = true;
            return true;
        }

        // return false: cancel the thread failed.
        bool Stop(bool immediately = true) {
            if (!IsAlive() || !thread_running_) {
                return true;
            }
            if (immediately) {
                pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
            }
            if (pthread_cancel(tid_) != 0) {
                return false;
            }
            thread_running_ = false;
            return true;
        }

        bool Wait() {
            if (!IsAlive() || !thread_running_) {
                return true;
            }
            if (pthread_join(tid_, NULL) != 0) {
                return false;
            }
            return true;
        }

        bool IsAlive() {
            int sig = pthread_kill(tid_, 0);
            if (ESRCH == sig) {
                return false;
            }
            // EINVAL: The value specified for the argument is not correct.
            //         the argument is 0, should not be return the EINVAL
            return true;
        }

    private:
        static void* hook(void* args) {
            reinterpret_cast<Thread*>(args)->Run();
            return NULL;
        }
    private:
        volatile std::atomic<bool> thread_running_;
        pthread_t tid_;

    protected:
        virtual void Run() = 0;
};

}  // end of namespace




#endif  //__THREAD_H_

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
