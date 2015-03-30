/***************************************************************************
 * 
 * Copyright (c) 2014 Aishuyu. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file connection_manager.cpp
 * @author aishuyu(asy5178@163.com)
 * @date 2014/11/24 14:29:52
 * @brief 
 *  
 **/


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <fcntl.h>
#include <netinet/tcp.h> 
#include <netdb.h>

#include "connection_manager.h"
#include "logger.h"

namespace libevrpc {

using std::string;

int32_t ConnectionManager::FAMILY_ = AF_UNSPEC;
int32_t ConnectionManager::SOCK_TYPE_ = SOCK_STREAM;

ConnectionManager::ConnectionManager() {
    Initialize();
}


ConnectionManager::~ConnectionManager() {
}

bool ConnectionManager::Initialize() {
    // here! should read config file
    strcpy(conf_host_, "127.0.0.1");
    strcpy(conf_port_, "9988");
    const int32_t MAXEPOLLSIZE = 1000;
    struct rlimit rt;
    rt.rlim_max = rt.rlim_cur = MAXEPOLLSIZE;

    if (setrlimit(RLIMIT_NOFILE, &rt) == -1) {
        LOGGING(ERROR, "set setrlimit falied size %d", MAXEPOLLSIZE);
        return false;
    }
    return true;
}

int32_t ConnectionManager::Socket(int32_t family, int32_t type, int32_t protocol) {
    int fd = -1;

    if ((fd = socket(family, type, protocol)) < 0) {
        return -1;
    }
    
    return fd;
}

 
int32_t ConnectionManager::TcpListen(
        const char *host, const char *port) {

    if (!ConfigSet(host, port)) {
        return -1;
    }

    int listenfd;
    struct addrinfo hints, *res, *ressave;

    bzero(&hints, sizeof(struct addrinfo));
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = FAMILY_;
    hints.ai_socktype = SOCK_TYPE_;

    if ((getaddrinfo(set_host_, set_port_, &hints, &res)) != 0) {  
        LOGGING(ERROR, "tcp_connect error for %s, %s", set_host_, set_port_);
        return -1;
    }

    ressave = res;
    const int on = 1;
    do {
        if (NULL == res) {
            return -1;
        }
        listenfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (listenfd < 0) {
            continue;       /* error, try next one */
        }
        
        if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
            LOGGING(ERROR, "setsockopt error listenfd %d", listenfd);
            return -1;
        }

        SetNonBlock(listenfd);
        if (bind(listenfd, res->ai_addr, res->ai_addrlen) == 0) {
            break;
        }
        close(listenfd);    
    } while ((res = res->ai_next) != NULL);

    if (res == NULL) {
        return -1;
    }

    if (listen(listenfd, 20/*LISTENQ*/) < 0) {
        LOGGING(ERROR, "listen error listenfd is %d\n", listenfd);
        return -1;
    }


    // if (addrlenp) {
    //     *addrlenp = res->ai_addrlen;    /* return size of protocol address */
    // }

    freeaddrinfo(ressave);
    return(listenfd);
}

int32_t ConnectionManager::TcpConnect(
        const char *host, const char *port) {
    if (!ConfigSet(host, port)) {
        return -1;
    }

    int32_t  sockfd;
    struct addrinfo hints, *res, *ressave;
       
    bzero(&hints, sizeof(struct addrinfo));
    hints.ai_family = FAMILY_;
    hints.ai_socktype = SOCK_TYPE_;

    if ((getaddrinfo(set_host_, set_port_, &hints, &res)) != 0) {  
        LOGGING(ERROR,"tcp_connect error for %s, %s", set_host_, set_port_);
        return -1;
    }
    
    ressave = res;
    do {
        sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (sockfd < 0) {
            continue;
        }
        
        if (connect(sockfd, res->ai_addr, res->ai_addrlen) == 0) {
            break;
        }
        close(sockfd);  /* ignore this one */
    } while ( (res = res->ai_next) != NULL);
    
    if (res == NULL) {    /* errno set from final connect() */
        LOGGING(ERROR, "tcp_connect error!");
        return -1;
    }
    freeaddrinfo(ressave);
    return(sockfd);
}

int32_t ConnectionManager::Accept(int fd, struct sockaddr_in &sa, int32_t addrlen) {
    int32_t new_fd;

    do {
        new_fd = accept(fd, (struct sockaddr *) &sa, (socklen_t *) &addrlen);

        if (new_fd < 0) {
#ifdef  EPROTO
            if (errno == EPROTO || errno == ECONNABORTED)
#else
            if (errno == ECONNABORTED)
#endif
                continue;
        }

    } while (false);
    return new_fd;
}

int32_t ConnectionManager::RecvMsg(int32_t fd, string& recv_msg_str) {
    recv_msg_str = "";

    const uint32_t MAXBUFLEN = 2048 * 2;
    char buf[MAXBUFLEN];
    memset(buf, 0, MAXBUFLEN);

    do {
        int32_t buf_len = recv(fd, buf, sizeof(buf) + 1, 0);
        if (buf_len < 0) {
            if (EAGAIN == errno || EINTR == errno) {
                break;
            } else {
                return -1;
            }
        } else if (0 == buf_len) {
            break;
        }


        if (buf_len > 0) {
            string new_buf(buf);
            recv_msg_str.append(new_buf);
            memset(buf, 0, MAXBUFLEN);
            continue;
        }
    } while (true);

    return 0;
}

int32_t ConnectionManager::SendMsg(int32_t fd, string& send_msg_str) {
    int32_t send_size = send_msg_str.size();
    const char* send_ptr = send_msg_str.c_str();

    do {
        int32_t buf_len = send(fd, send_ptr, send_size + 1, 0);
        if (buf_len < 0) {
            if (EINTR == errno) {
                LOGGING(ERROR, "send error errno is EINTR");
                return -1;
            } 
            if (EAGAIN == errno) {
                sleep(1);
                continue;
            }
            return -1;
        }
        if (buf_len >= send_size) {
            return buf_len;
        }

        send_size -= buf_len;
        send_ptr += buf_len;
    } while(send_size > 0);

    return 0;

}


int32_t ConnectionManager::EpollInit(int32_t listenfd) {
    // max poll size
    const int32_t  MAXEPOLLSIZE = 10000;

    ep_create_fd_ = epoll_create(MAXEPOLLSIZE);
    ev_.events = EPOLLIN | EPOLLET;
    ev_.data.fd = listenfd;
    if (epoll_ctl(ep_create_fd_, EPOLL_CTL_ADD, listenfd, &ev_) < 0) {
        LOGGING(ERROR, "epoll set insertion error: fd = %u", listenfd);
        return -1;
    }
    return 0;
}

int32_t ConnectionManager::EpollWait(int32_t max_events, struct epoll_event* events) {
    int32_t ready_nfds = epoll_wait(ep_create_fd_, events, max_events, -1);

    if (ready_nfds < 0) {
        LOGGING(ERROR, "epoll wait error return fd is %d\n", ready_nfds);
        return -1;
    }
    return ready_nfds;

}

int32_t ConnectionManager::EpollNewConnect(int32_t listenfd) {

    struct sockaddr_in remote_addr;
    int32_t addrlen = sizeof(struct sockaddr_in);
    int32_t connect_fd = -1;
    while ((connect_fd = Accept(listenfd, remote_addr, addrlen)) > 0) {
        SetNonBlock(connect_fd);
        ev_.events = EPOLLIN | EPOLLET;
        ev_.data.fd = connect_fd;
        if (epoll_ctl(ep_create_fd_, EPOLL_CTL_ADD, connect_fd, &ev_) < 0) {
            LOGGING(ERROR, "epoll_ctl error\n");
            return -1;
        }
    }

    if (-1 == connect_fd) {
        if (errno != EAGAIN  && errno != EINTR) {
#ifdef  EPROTO
            if (errno != EPROTO && errno != ECONNABORTED)
#else
            if (errno != ECONNABORTED)
#endif
                return -1;
        }
    }

    return 0;
}
 
int32_t ConnectionManager::EpollRecvMsg(int32_t fd, string& recv_msg_str) {

    if (RecvMsg(fd, recv_msg_str) < 0) {
        return -1;
    }

    ev_.data.fd = fd;
    ev_.events = EPOLLOUT | EPOLLET;

    if (epoll_ctl(ep_create_fd_, EPOLL_CTL_MOD, fd, &ev_) == -1) {
        LOGGING(ERROR, "epoll_ctl error");
        return -1;
    }

    return 0;
}

int32_t ConnectionManager::EpollSendMsg(int32_t fd, string& send_msg_str) {

    if (SendMsg(fd, send_msg_str) < 0) {
        return -1;
    }

    //ev_.data.fd = fd;
    //ev_.events = EPOLLIN | EPOLLOUT;

    //if (epoll_ctl(ep_create_fd_, EPOLL_CTL_DEL, fd, &ev_) == -1) {
    //    return -1;
    //}

    // close(fd);

    return fd;
}

int32_t ConnectionManager::EpollClose(int32_t fd) {
    ev_.data.fd = fd;
    ev_.events = EPOLLIN | EPOLLOUT;

    if (epoll_ctl(ep_create_fd_, EPOLL_CTL_DEL, fd, &ev_) == -1) {
        LOGGING(ERROR, "epoll_ctl error");
        return -1;
    }

    close(fd);
    return 0;

}

bool ConnectionManager::SetNonBlock(int32_t sock) {
    int32_t opts = fcntl(sock, F_GETFL);

    if (opts < 0) {
        LOGGING(ERROR, "set non block failed! fcntl(sock,GETFL).");
        return false;
    }

    opts = opts | O_NONBLOCK;
    if (fcntl(sock, F_SETFL, opts) < 0) {
        LOGGING(ERROR, "set non block failed! fcntl(sock,GETFL).");
        return false;
    }
    return true;
}


bool ConnectionManager::ConfigSet(const char* host, const char* port) {

    uint32_t conf_host_size = strlen(conf_host_);
    uint32_t conf_port_size = strlen(conf_port_);

    uint32_t param_host_size = strlen(host);
    uint32_t param_port_size = strlen(port);


    if (conf_host_size > SOCKINFO_MAXSIZE || param_host_size > SOCKINFO_MAXSIZE) {
        LOGGING(ERROR, "invalid host  conf host: %s, param host: %s\n",  conf_host_, host);
        return false;
    }
    if (conf_port_size > SOCKINFO_MAXSIZE || param_port_size > SOCKINFO_MAXSIZE) {
        LOGGING(ERROR, "invalid port  conf port: %s, param port: %s\n", conf_port_, port);
        return false;
    }

    // the user set is priority
    if (0 != param_host_size && 0 != param_port_size) {
        strcpy(set_host_, host);
        strcpy(set_port_, port);
    } else if (0 != conf_host_size && 0 != conf_port_size) {
        strcpy(set_host_, conf_host_);
        strcpy(set_port_, conf_port_); 
    } else {
        return false;
    }
    return true;

}

}  // end of namespace libevrpc





/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
