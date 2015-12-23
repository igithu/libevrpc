/***************************************************************************
 * 
 * Copyright (c) 2015 aishuyu, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file pthread_rwlock.h
 * @author aishuyu(asy5178@163.com)
 * @date 2015/01/04 17:09:52
 * @brief 
 *  
 **/




#ifndef  __PTHREAD_LOCK_H_
#define  __PTHREAD_LOCK_H_


#include <errno.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>


namespace PUBLIC_UTIL {

class RWLock {
    public:
        RWLock() : create_success_(false)/*, rwlock_(PTHREAD_MUTEX_INITIALIZER)*/ {
            int ret = pthread_rwlock_init(&rwlock_, NULL);
            if (0 == ret) {
                create_success_ = true;
            } else {
                fprintf(stderr, "pthread init rwlock failed! the error num is %d\n", ret);
            }
        }

        ~RWLock() {
            int ret = pthread_rwlock_destroy(&rwlock_);
            if (0 != ret) {
                 fprintf(stderr,"pthread destroy rwlock failed! the error num is %d\n", ret);
            }
        }

        bool IsSuccess() {
            return create_success_;
        }

        pthread_rwlock_t& MLock() {
            return rwlock_;
        }
    private:
        bool create_success_;
        pthread_rwlock_t rwlock_;
};


class ReadLockGuard {
    public:
        ReadLockGuard(RWLock &rwlock, bool trylock = false) 
            : rwlock_(rwlock.MLock()), lock_success_(false) {
            if (false == trylock) {
                int ret = pthread_rwlock_rdlock(&rwlock_);
                if (0 == ret) {
                    lock_success_ = true;
                } else {
                    fprintf(stderr, "pthread lock rwlock failed! the error num is %d\n", ret);
                }
            } else {
                // bad desgin?
                int ret = 0;
                while ((ret = pthread_rwlock_tryrdlock(&rwlock_)) == EBUSY) {
                    usleep(1);
                }
                if (0 == ret) {
                    lock_success_ = true;
                } else {
                    fprintf(stderr, "pthread trylock rwlock failed! the error num is %d\n", ret);
                }
            }
        }

        ~ReadLockGuard() {
            int ret = pthread_rwlock_unlock(&rwlock_);
            if (0 != ret) {
                fprintf(stderr, "pthread unlock rwlock failed! the error num is %d\n", ret);
            }
        }

        bool IsSuccess() {
            return lock_success_;
        }
    private:
        pthread_rwlock_t& rwlock_;
        bool lock_success_;

};


class WriteLockGuard {
    public:
        WriteLockGuard(RWLock &rwlock, bool trylock = false) 
            : rwlock_(rwlock.MLock()), lock_success_(false) {
            if (false == trylock) {
                int ret = pthread_rwlock_wrlock(&rwlock_);
                if (0 == ret) {
                    lock_success_ = true;
                } else {
                    fprintf(stderr, "pthread lock rwlock failed! the error num is %d\n", ret);
                }
            } else {
                // bad desgin?
                int ret = 0;
                while ((ret = pthread_rwlock_trywrlock(&rwlock_)) == EBUSY) {
                    usleep(1);
                }
                if (0 == ret) {
                    lock_success_ = true;
                } else {
                    fprintf(stderr, "pthread trylock rwlock failed! the error num is %d\n", ret);
                }
            }
        }

        ~WriteLockGuard() {
            int ret = pthread_rwlock_unlock(&rwlock_);
            if (0 != ret) {
                fprintf(stderr, "pthread unlock rwlock failed! the error num is %d\n", ret);
            }
        }

        bool IsSuccess() {
            return lock_success_;
        }
    private:
        pthread_rwlock_t& rwlock_;
        bool lock_success_;

};

}  // end of namespace PUBLIC_UTIL













#endif  //__PTHREAD_LOCK_H_

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
