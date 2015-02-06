/***************************************************************************
 * 
 * Copyright (c) 2015 aishuyu, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file socket_util.cpp
 * @author aishuyu(asy5178@163.com)
 * @date 2015/01/28 14:33:35
 * @brief 
 *  
 **/


#include "socket_util.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <fcntl.h>
#include <netinet/tcp.h> 
#include <netdb.h>

#include "../log/ds_log.h"

namespace libevrpc {

using std::string;

bool SetNonBlock(int32_t sock) {
    int32_t opts = fcntl(sock, F_GETFL);

    if (opts < 0) {
        DS_LOG(ERROR, "set non block failed! fcntl(sock,GETFL).");
        return false;
    }

    opts = opts | O_NONBLOCK;
    if (fcntl(sock, F_SETFL, opts) < 0) {
        DS_LOG(ERROR, "set non block failed! fcntl(sock,GETFL).");
        return false;
    }
    return true;
}

int32_t Socket(int32_t family, int32_t type, int32_t protocol) {

    int32_t fd = -1;
    if ((fd = socket(family, type, protocol)) < 0) {
        return -1;
    }
    return fd;
}

int32_t TcpListen(const char *host, const char *port, int32_t family) {
    int32_t listenfd;
    struct addrinfo hints, *res, *ressave;

    bzero(&hints, sizeof(struct addrinfo));
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = family;
    hints.ai_socktype = SOCK_STREAM;

    if ((getaddrinfo(host, port, &hints, &res)) != 0) {  
        DS_LOG(ERROR, "tcp_connect error for %s, %s", host, port);
        return -1;
    }

    ressave = res;
    const int on = 1;
    do {
        if (NULL == res) {
            return -1;
        }
        listenfd = Socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (listenfd < 0) {
            continue;       /* error, try next one */
        }
        
        if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
            DS_LOG(ERROR, "setsockopt error listenfd %d", listenfd);
            return -1;
        }

        SetNonBlock(listenfd);
        if (bind(listenfd, res->ai_addr, res->ai_addrlen) == 0) {
            break;
        }
        close(listenfd);    
    } while ((res = res->ai_next) != NULL);

    if (NULL == res) {
        return -1;
    }

    if (listen(listenfd, 20/*LISTENQ*/) < 0) {
        DS_LOG(ERROR, "listen error listenfd is %d\n", listenfd);
        return -1;
    }


    // if (addrlenp) {
    //     *addrlenp = res->ai_addrlen;    /* return size of protocol address */
    // }

    freeaddrinfo(ressave);
    return listenfd;

}

int32_t TcpConnect(const char *host, const char *port, int32_t family) {
    int32_t  sockfd;
    struct addrinfo hints, *res, *ressave;
       
    bzero(&hints, sizeof(struct addrinfo));
    hints.ai_family = family;
    hints.ai_socktype = SOCK_STREAM;

    if ((getaddrinfo(host, port, &hints, &res)) != 0) {  
        DS_LOG(ERROR,"tcp_connect error for %s, %s", host, port);
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
    } while ((res = res->ai_next) != NULL);
    
    if (res == NULL) {    /* errno set from final connect() */
        DS_LOG(ERROR, "tcp_connect error!");
        return -1;
    }
    freeaddrinfo(ressave);
    return sockfd;

}

int32_t Accept(int fd, struct sockaddr_in &sa, int32_t addrlen) {
    int32_t new_fd;
    do {
        new_fd = accept(fd, (struct sockaddr *) &sa, (socklen_t *) &addrlen);
        SetNonBlock(new_fd);

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

int32_t RecvMsg(int32_t fd, std::string& recv_msg_str) {
    recv_msg_str = "";

    const uint32_t MAXBUFLEN = 512;
    char buf[MAXBUFLEN];
    memset(buf, 0, MAXBUFLEN);

    do {
        int32_t buf_len = recv(fd, buf, sizeof(buf) + 1, 0);
        if (buf_len < 0) {
            if (EAGAIN == errno || EINTR == errno) {
                //DS_LOG(ERROR, "EAGAIN or EINTR in RecvMsg!");
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

int32_t SendMsg(int32_t fd, std::string& send_msg_str) {
    int32_t send_size = send_msg_str.size();
    const char* send_ptr = send_msg_str.c_str();

    do {
        int32_t buf_len = send(fd, send_ptr, send_size + 1, 0);
        if (buf_len < 0) {
            if (EINTR == errno) {
                DS_LOG(ERROR, "send error errno is EINTR");
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

}  // end of namespace libevrpc









/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
