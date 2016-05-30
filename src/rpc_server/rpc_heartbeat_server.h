/***************************************************************************
 *
 * Copyright (c) 2016 aishuyu, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file rpc_heartbeat_server.h
 * @author aishuyu(asy5178@163.com)
 * @date 2016/04/04 22:08:26
 * @brief
 *
 **/




#ifndef __RPC_HEARTBEAT_SERVER_H
#define __RPC_HEARTBEAT_SERVER_H

// #include <atmoic>
#include <ev.h>

#include "util/thread.h"

namespace libevrpc {

class RpcHeartbeatServer : public Thread {
    public:
        RpcHeartbeatServer(
                const char* hb_host,
                const char* hb_port,
                const char* config_file);
        ~RpcHeartbeatServer();

        bool InitHeartbeatServer();

        virtual void Run();

        /*
         * recv the hearbeat in libev and put the hb info
         * into connection timer manager
         */
        static void HeartBeatProcessor(struct ev_loop *loop, struct ev_io *watcher, int revents);

    private:
        char* hb_host_;
        char* hb_port_;
        char* config_file_;

        bool hb_running_;

        struct ev_loop *epoller_;
        struct ev_io socket_watcher_;

};

}  // end of namespace libevrpc




#endif // __RPC_HEARTBEAT_SERVER_H



/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
