/***************************************************************************
 *
 * Copyright (c) 2014 aishuyu.com, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file pthread_mutex.h
 * @author aishuyu(asy5178@163.com)
 * @date 2014/11/16 22:45:00
 * @brief
 *
 **/




#ifndef  __PTHREAD_MUTEX_H_
#define  __PTHREAD_MUTEX_H_

#include <errno.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>


namespace libevrpc {

class Mutex {
    public:
        Mutex() : create_success_(false)/*, mutex_(PTHREAD_MUTEX_INITIALIZER)*/ {
            int ret = pthread_mutex_init(&mutex_, NULL);
            if (0 == ret) {
                create_success_ = true;
            } else {
                fprintf(stderr, "pthread init mutex failed! the error num is %d\n", ret);
            }
        }

        ~Mutex() {
            int ret = pthread_mutex_destroy(&mutex_);
            if (0 != ret) {
                 fprintf(stderr,"pthread destroy mutex failed! the error num is %d\n", ret);
            }
        }

        bool IsSuccess() {
            return create_success_;
        }

        pthread_mutex_t& MMutex() {
            return mutex_;
        }
    private:
        bool create_success_;
        pthread_mutex_t mutex_;
};


class MutexLockGuard {
    public:
        MutexLockGuard(Mutex &mutex, bool trylock = false) 
            : mutex_(mutex.MMutex()), lock_success_(false) {
            if (false == trylock) {
                int ret = pthread_mutex_lock(&mutex_);
                if (0 == ret) {
                    lock_success_ = true;
                } else {
                    fprintf(stderr, "pthread lock mutex failed! the error num is %d\n", ret);
                }
            } else {
                // bad desgin?
                int ret = 0;
                while ((ret = pthread_mutex_trylock(&mutex_)) == EBUSY) {
                    usleep(1);
                }
                if (0 == ret) {
                    lock_success_ = true;
                } else {
                    fprintf(stderr, "pthread trylock mutex failed! the error num is %d\n", ret);
                }
            }
        }

        ~MutexLockGuard() {
            int ret = pthread_mutex_unlock(&mutex_);
            if (0 != ret) {
                fprintf(stderr, "pthread unlock mutex failed! the error num is %d\n", ret);
            }
        }

        bool IsSuccess() {
            return lock_success_;
        }
    private:
        pthread_mutex_t& mutex_;
        bool lock_success_;

};


}  // end of namespace

#endif  //__PTHREAD_MUTEX_H_

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
