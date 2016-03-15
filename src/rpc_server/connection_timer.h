/***************************************************************************
 *
 * Copyright (c) 2015 aishuyu, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file connection_timer.h
 * @author aishuyu(asy5178@163.com)
 * @date 2016/03/08 23:39:34
 * @brief
 *
 **/




#ifndef __CONNECTION_TIMER_H
#define __CONNECTION_TIMER_H

#include <vector>

#include "util/thread.h"

namespace libevrpc {

struct ConnectionInfo {
    time_t expire_time;
};

class ConnectionTimer : public Thread{
    public:
        ConnectionTimer();
        ~ConnectionTimer();

        virtual void Run();

};

}  // end of namespace libevrpc


#endif // __CONNECTION_TIMER_H



/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
