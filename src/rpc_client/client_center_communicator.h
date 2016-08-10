/***************************************************************************
 *
 * Copyright (c) 2016 aishuyu, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file client_center_communicator.h
 * @author aishuyu(asy5178@163.com)
 * @date 2016/08/10 20:40:11
 * @brief
 *
 **/




#ifndef __CLIENT_CENTER_COMMUNICATOR_H
#define __CLIENT_CENTER_COMMUNICATOR_H

#include "util/thread.h"

namespace libevrpc {

class ClientCenterCommunicator : public Thread {
    public:
        ClientCenterCommunicator();

        ~ClientCenterCommunicator();

        bool CreateCenterCommunication();

        virtual void Run();

};

}  // end of namespace libevrpc



#endif // __CLIENT_CENTER_COMMUNICATOR_H



/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
