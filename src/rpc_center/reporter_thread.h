/***************************************************************************
 *
 * Copyright (c) 2016 aishuyu, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file reporter_thread.h
 * @author aishuyu(asy5178@163.com)
 * @date 2016/07/16 22:42:27
 * @brief
 *
 **/




#ifndef __REPORTER_THREAD_H
#define __REPORTER_THREAD_H

#include "util/thread.h"

namespace libevrpc {

/*
 * 与Leader机器通信, 交换信息
 */
class ReporterThread : public Thread {
    public:
        ReporterThread();
        ~ReporterThread();

        bool StartReporter(
                const char* leader_addr,
                const char* leader_port);

        virtual void Run();

    private:
        char* leader_addr_;
        char* leader_port_;

        bool reporter_running_;

};

}  // end of namespace libevrpc




#endif // __REPORTER_THREAD_H



/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
