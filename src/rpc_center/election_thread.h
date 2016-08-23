/***************************************************************************
 *
 * Copyright (c) 2016 aishuyu, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file election_thread.h
 * @author aishuyu(asy5178@163.com)
 * @date 2016/07/22 15:47:05
 * @brief
 *
 **/




#ifndef __ELECTION_THREAD_H
#define __ELECTION_THREAD_H

#include <string>
#include <atomic>

#include "center_proto/centers.pb.h"
#include "util/thread.h"
#include "util/pthread_mutex.h"

namespace libevrpc {

typedef struct ElectionItem EL_ITEM;
struct ElectionItem {
    int32_t conn_fd;
    CentersProto centers_proto;
    EL_ITEM* prev;
    EL_ITEM* next;
};

typedef struct ElectionQueue EQ;
struct ElectionQueue {
    EL_ITEM* head;
    EL_ITEM* tail;
};

/*
 * 选举时候，启动该线程；选举结束，结束该线程
 */
class ElectionThread : public Thread {
    public:
        ElectionThread();
        ~ElectionThread();

        virtual void Run();
        void StopThread();

        bool PushElectionMessage(int32_t fd, const CentersProto& centers_proto);
        ElectionItem* PopElectionMessage();

    private:
        void Destory();

    private:
        volatile std::atomic<bool> running_;
        EQ* election_q_;

        Mutex eq_mutex_;
};


}  // end of namespace libevrpc




#endif // __ELECTION_THREAD_H



/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
