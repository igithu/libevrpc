/***************************************************************************
 *
 * Copyright (c) 2016 aishuyu, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file center_client_heartbeat.h
 * @author aishuyu(asy5178@163.com)
 * @date 2016/09/07 20:52:26
 * @brief
 *
 **/




#ifndef __CENTER_CLIENT_HEARTBEAT_H
#define __CENTER_CLIENT_HEARTBEAT_H

#include "util/thread.h"

namespace libevrpc {

class CenterClientHeartbeat : public Thread {
    public:
        CenterClientHeartbeat();

        ~CenterClientHeartbeat();

        virtual void Run();

    private:
};

}  // end of namespace libevrpc




#endif // __CENTER_CLIENT_HEARTBEAT_H



/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
