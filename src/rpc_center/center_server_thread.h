/***************************************************************************
 *
 * Copyright (c) 2016 aishuyu, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file center_server_thread.h
 * @author aishuyu(asy5178@163.com)
 * @date 2016/07/17 02:29:13
 * @brief
 *
 **/




#ifndef __CENTER_SERVER_THREAD_H
#define __CENTER_SERVER_THREAD_H

#include "util/thread.h"

namespace libevrpc {

class CenterServerThread : public Thread {
    public:
        CenterServerThread();
        ~CenterServerThread();

        virtual void Run();

};

}  // end of namespace libevrpc




#endif // __CENTER_SERVER_THREAD_H



/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
