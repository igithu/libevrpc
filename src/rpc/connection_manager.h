/***************************************************************************
 * 
 * Copyright (c) 2014 Aishuyu. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file connection_manager.h
 * @author aishuyu(asy5178@163.com)
 * @date 2014/11/24 14:24:05
 * @brief 
 *  
 **/




#ifndef  __CONNECTION_MANAGER_H_
#define  __CONNECTION_MANAGER_H_

#include <sys/epoll.h> 
#include <sys/socket.h>

#include <string>

#define SOCKINFO_MAXSIZE 128

namespace libevrpc {

class ConnectionManager {
    public:

        ConnectionManager();

        ~ConnectionManager();

        bool Initialize();

    public:

        // wrapper socket, imitate the <Unix socket> code
        int32_t Socket(int32_t family = FAMILY_, int32_t type = SOCK_TYPE_, int32_t protocol = 0);

        int32_t TcpListen(const char *host = "", const char *port = "");

        int32_t TcpConnect(const char *host = "", const char *port = "");

        int32_t Accept(int fd, struct sockaddr_in &sa, int32_t addrlen);

        int32_t RecvMsg(int32_t fd, std::string& recv_msg_str);

        int32_t SendMsg(int32_t fd, std::string& send_msg_str);

        // the prefix is Epoll: epoll connect api
        int32_t EpollInit(int32_t listenfd); 

        int32_t EpollWait(int32_t max_events, struct epoll_event* events);

        int32_t EpollNewConnect(int32_t listenfd);

        // read and write 
        int32_t EpollRecvMsg(int32_t fd, std::string& recv_msg_str);

        int32_t EpollSendMsg(int32_t fd, std::string& send_msg_str);

        int32_t EpollClose(int32_t fd);

        bool SetNonBlock(int32_t sock);

    private:

        bool ConfigSet(const char* host, const char* port);

    private:
        struct epoll_event ev_;
        int32_t ep_create_fd_;

        char conf_host_[SOCKINFO_MAXSIZE];
        char conf_port_[SOCKINFO_MAXSIZE];

        char set_host_[SOCKINFO_MAXSIZE];
        char set_port_[SOCKINFO_MAXSIZE];

        static int32_t FAMILY_;
        static int32_t SOCK_TYPE_;


};

}  // end of namespace libevrpc




#endif  //__CONNECTION_MANAGER_H_

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
