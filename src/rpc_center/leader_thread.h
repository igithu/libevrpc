/***************************************************************************
 *
 * Copyright (c) 2016 aishuyu, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file leader_thread.h
 * @author aishuyu(asy5178@163.com)
 * @date 2016/08/25 20:45:56
 * @brief
 *
 **/




#ifndef __LEADER_THREAD_H
#define __LEADER_THREAD_H


#include "center_proto/centers.pb.h"
#include "util/thread.h"
#include "util/pthread_mutex.h"

namespace libevrpc {

typedef struct FollowerItem FW_ITEM;
struct FollowerItem {
    int32_t conn_fd;
    CentersProto centers_proto;
    FW_ITEM* prev;
    FW_ITEM* next;
};

typedef struct FollowerQueue FQ;
struct FollowerQueue {
    FW_ITEM* head;
    FW_ITEM* tail;
};

/*
 * Center进入Leading状态下启动 leader 线程异步处理 Follower发送来的请求
 */
class LeaderThread : public Thread {
    public:
        LeaderThread();
        ~LeaderThread();

        virtual void Run();
        void StopThread();

        bool PushFollowerMessage(int32_t fd, const CentersProto& centers_proto);
        FollowerItem* PopFollowerMessage();

    private:
        void Destory();

    private:
        volatile std::atomic<bool> running_;
        FQ* follower_q_;

        Mutex fq_mutex_;
};


}  // end of namespace libevrpc







#endif // __LEADER_THREAD_H



/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
