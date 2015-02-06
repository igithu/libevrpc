/***************************************************************************
 * 
 * Copyright (c) 2015 aishuyu, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file libev_connector.h
 * @author aishuyu(asy5178@163.com)
 * @date 2015/01/27 15:12:51
 * @brief 
 *  
 **/




#ifndef  __LIBEV_CONNECTOR_H_
#define  __LIBEV_CONNECTOR_H_

#define SOCKINFO_MAXSIZE 128

#include <ev.h>

namespace libevrpc {

class LibevConnector {
    public:
        // ctor
        LibevConnector();

        // dtor
        ~LibevConnector();

        bool Initialize(const char *host, const char *port);

        void LibevLoop();

    private:
        static void AcceptCb(struct ev_loop *loop, struct ev_io *watcher, int revents);

        static void ProcessCb(struct ev_loop *loop, struct ev_io *watcher, int revents);

    private:

        struct ev_loop *epoller_;

        struct ev_io socket_watcher_;
};


}  // end of namespace libevrpc




#endif  //__LIBEV_CONNECTOR_H_

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
