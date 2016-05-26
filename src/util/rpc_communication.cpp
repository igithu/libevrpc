/***************************************************************************
 *
 * Copyright (c) 2015 aishuyu, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file rpc_communication.cpp
 * @author aishuyu(asy5178@163.com)
 * @date 2015/09/03 22:12:10
 * @brief
 *
 **/

#include "rpc_communication.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <netdb.h>

#include "rpc_util.h"

namespace libevrpc {

using std::string;

bool NonBlockMode(int32_t sock, bool mode) {
    int32_t opts = fcntl(sock, F_GETFL);

    if (opts < 0) {
        PrintErrorInfo("Get Flag sock mode failed!\n");
        return false;
    }

    unsigned long ul = 1;
    if (mode) {
        opts |= O_NONBLOCK;
    } else {
        opts |= ~O_NONBLOCK;
        ul = 0;
    }
    if (fcntl(sock, F_SETFL, opts) < 0) {
        /*
         * when use fcntl always set mode failed,
         * use the ioctl!
         *
         * fcntl & ioctl differnce:
         *             fcntl: attribute of file
         *             ioctl: attribute of device
         */
        if (ioctl(sock, FIONBIO, &ul) < 0) {
            PrintErrorInfo("Call fcntl failed!\n");
            return false;
        }
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


int32_t TcpListen(const char *host, const char *port, bool non_block, int32_t family) {
    struct addrinfo hints, *res = NULL, *ressave = NULL;

    bzero(&hints, sizeof(struct addrinfo));
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = family;
    hints.ai_socktype = SOCK_STREAM;

    if ((getaddrinfo(host, port, &hints, &res)) != 0) {
        fprintf(stderr, "Tcp_connect error for %s, %s.\n", host, port);
        return -1;
    }

    ressave = res;
    int32_t listenfd;
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
            fprintf(stderr, "Setsockopt error listenfd %d.\n", listenfd);
            continue;
        }

        if (bind(listenfd, res->ai_addr, res->ai_addrlen) == 0) {
            break;
        }
        close(listenfd);
    } while ((res = res->ai_next) != NULL);

    if (NULL == res) {
        return -1;
    }

    if (listen(listenfd, 20/*LISTENQ*/) < 0) {
        fprintf(stderr, "Listen error listenfd is %d\n", listenfd);
        freeaddrinfo(ressave);
        return -1;
    }

    freeaddrinfo(ressave);
    return listenfd;
}

int32_t TcpConnect(const char *host, const char *port, const int32_t conn_overtime, int32_t family) {
    struct addrinfo hints, *res = NULL, *ressave = NULL;

    bzero(&hints, sizeof(struct addrinfo));
    hints.ai_family = family;
    hints.ai_socktype = SOCK_STREAM;

    if ((getaddrinfo(host, port, &hints, &res)) != 0) {
        fprintf(stderr, "Tcp_connect error for %s, %s\n", host, port);
        return -1;
    }

    int32_t  sockfd, conn_ret;
    ressave = res;
    do {
        sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (sockfd < 0) {
            continue;
        }

        int32_t optval = 1;
        if (setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval))) {
            continue;
        }

        if (conn_overtime > 0) {
            NonBlockMode(sockfd, true);
        }

        conn_ret = connect(sockfd, res->ai_addr, res->ai_addrlen);
        /*
         * 0: connect successfully, conn_ret < 0 and EINPROGRESS: connecting
         *
         */
        if (conn_overtime > 0 && conn_ret < 0 && errno == EINPROGRESS) {
            fd_set fs;
            FD_ZERO(&fs);
            FD_SET(sockfd, &fs);
            struct timeval overtime;
            overtime.tv_sec = conn_overtime / 1000;
            overtime.tv_usec = conn_overtime % 1000;
            int ret = select(sockfd + 1, NULL, &fs, NULL, &overtime);
            if (0 == ret) {
                fprintf(stderr, "Connect server time out, over time is %d\n", conn_overtime);
                sockfd = TCP_CONN_TIMEOUT;
                break;
            } else if (-1 != ret) {
                // sockfd = -1;
                break;
            }
        } else if (0 == conn_ret) {
            break;
        }

        close(sockfd);  /* ignore this one */
    } while ((res = res->ai_next) != NULL);

    if (conn_overtime > 0 && !NonBlockMode(sockfd, false)) {
        PrintErrorInfo("TCP connection failed!");
        freeaddrinfo(ressave);
        return -1;
    }

    /* errno set from final connect() */
    if (NULL == res) {
        fprintf(stderr, "TCP connection error! the errno is: %s\n", strerror(errno));
        freeaddrinfo(ressave);
        return -1;
    }

    freeaddrinfo(ressave);
    return sockfd;
}

int32_t UdpConnect(const char *host, const char *port, int32_t family) {
    struct addrinfo hints, *res, *ressave;
    bzero(&hints, sizeof(struct addrinfo));
    hints.ai_family = family;
    hints.ai_socktype = SOCK_DGRAM;

    int32_t sockfd, n;
    if ((n = getaddrinfo(host, port, &hints, &res)) != 0) {
        fprintf(stderr, "Tcp_connect error for %s, %s\n", host, port);
        return -1;
    }
    ressave = res;
    do {
        sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (sockfd < 0) {
            continue;   /* ignore this one */
        }
        if (connect(sockfd, res->ai_addr, res->ai_addrlen) == 0) {
            break;      /* success */
        }
        close(sockfd);  /* ignore this one */
    } while ( (res = res->ai_next) != NULL);

    if (NULL == res) {
        fprintf(stderr, "UDP  connection error! the errno is: %s\n", strerror(errno));
        freeaddrinfo(ressave);
        return -1;
    }
    freeaddrinfo(ressave);
    return(sockfd);
}

int32_t UdpServerInit(const char *host, const char *port) {
    int32_t sockfd = Socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        fprintf(stderr, "Socket error! the errno is: %s\n", strerror(errno));
        return -1;
    }

    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(host);
    // servaddr.sin_port = inet_addr(port);
    servaddr.sin_port = htons(8899);

    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        fprintf(stderr, "Socket error! the errno is: %s\n", strerror(errno));
        return -1;
    }
    return sockfd;
}

int32_t UdpClientInit(const char *server_host, const char *port, struct sockaddr_in *servaddr) {
    int32_t sockfd = Socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        fprintf(stderr, "Socket error! the errno is: %s\n", strerror(errno));
        return -1;
    }

    bzero(servaddr, sizeof(servaddr));
    servaddr->sin_family = AF_INET;
    // servaddr->sin_port = inet_addr(port);
    servaddr->sin_port = htons(8899);

    if (inet_pton(AF_INET, server_host, &(servaddr->sin_addr)) <= 0) {
        fprintf(stderr, "inet_pton error! the errno is: %s\n", strerror(errno));
        return -1;
    }

    return sockfd;
}

int32_t Accept(int fd, struct sockaddr_in &sa, int32_t addrlen, bool non_block) {
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

    int32_t optval = 1;
    // if client lose connection
    if (setsockopt(new_fd, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval)) < 0) {
        printf("Accept error! cann't set SO_KEEPALIVE! the errno is: %s.", strerror(errno));
        close(new_fd);
        return -1;
    }

    optval = 60; // 1min interval
    // if client lose connection
    if (setsockopt(new_fd, IPPROTO_TCP, TCP_KEEPINTVL, &optval, sizeof(optval)) < 0) {
        printf("Accept error! cann't set TCP_KEEPINTVL! the errno is: %s.", strerror(errno));
        close(new_fd);
        return -1;
    }

    // 1s interval keepalive
    /*if (setsockopt(new_fd, IPPROTO_TCP, TCP_KEEPIDLE, &optval, sizeof(optval)) < 0) {
        close(new_fd);
        return -1;
    }*/

    optval = 3;
    // 3 times retry
    if (setsockopt(new_fd, IPPROTO_TCP, TCP_KEEPCNT, &optval, sizeof(optval)) < 0) {
        fprintf(stderr, "Accept error! cann't set TCP_KEEPCNT. the errno is: %s.\n", strerror(errno));
        close(new_fd);
        return -1;
    }

    optval = 1;
    if (setsockopt(new_fd, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval))) {
        fprintf(stderr, "Accept error! cann't set TCP_NODELAY. the errno is: %s.\n", strerror(errno));
        close(new_fd);
        return -1;
    }

    return new_fd;
}


int32_t RpcRecv(int32_t fd, std::string& recv_info_str, bool need_closed) {
    char recv_buf[MetaSize];
    /*
     * transfer_id : when h_code less 0, is RpcSend the info
     *               others, is error code or nothing
     */
    int32_t transfer_id;

    do {
        memset(recv_buf, 0, sizeof(recv_buf));
        int32_t buf_len = recv(fd, recv_buf, MetaSize, 0);
        if (-1 == buf_len && EAGAIN == errno) {
            /*
             * Resource temporarily unavailable! wait and recv again!
             */
            usleep(1);
            continue;
        } else if (buf_len < 0) {
            fprintf(stderr, "Call RpcRecv func error!, buf len is %d, the errno is %s.\n", buf_len, strerror(errno));
            transfer_id = ERROR_RECV;
            break;
        } else if (0 == buf_len) {
            transfer_id = 0;
            break;
        }
        MetaData* md_ptr = (MetaData*)recv_buf;
        recv_info_str.append(md_ptr->body, strlen(md_ptr->body));
        if (md_ptr->h_code < 0) {
            transfer_id = md_ptr->h_code * -1;
            break;
        }
    } while(true);

    if (need_closed) {
        close(fd);
    }

    return transfer_id;
}

int32_t RpcSend(int32_t fd, int32_t transfer_id, std::string& send_info_str, bool need_closed) {
    /*
     * call id is less zero, error call id
     */
    if (transfer_id < 0) {
        return -1;
    } else if (0 == transfer_id) {
        /*
         * transfer_id is 0, in fact make the h_code be -1
         * the user of RpcRecv should ingore it. because it
         * means noting
         */
        transfer_id = 1;
    }

    const char* send_ptr = send_info_str.c_str();
    int32_t block_num = send_info_str.size() / (BODY_SIZE - 1) + 1;

    MetaData meta_data;
    for (int i = 0; i < block_num - 1; ++i) {
        memset(&meta_data, 0, sizeof(meta_data));
        memcpy(meta_data.body, send_ptr + i * (BODY_SIZE - 1), BODY_SIZE - 1);
        (meta_data.body)[BODY_SIZE - 1] = '\0';
        meta_data.h_code = i;
        if (send(fd, &meta_data, MetaSize, 0) < 0) {
            PrintErrorInfo("Send Meta Data failed!\n");
            close(fd);
            return -1;
        }
    }

    int32_t rest_len = send_info_str.size() % (BODY_SIZE - 1) ;
    if (0 != rest_len) {
        memset(&meta_data, 0, sizeof(meta_data));
        memcpy(meta_data.body, send_ptr + (block_num - 1)* (BODY_SIZE  - 1), rest_len);
        (meta_data.body)[rest_len] = '\0';
        meta_data.h_code = -1 * transfer_id;
        if (send(fd, &meta_data, MetaSize, 0) < 0) {
            PrintErrorInfo("Send Meta Data failed!\n");
            close(fd);
            return -1;
        }
    }

    if (need_closed) {
        close(fd);
    }

    return 0;
}

int32_t RpcRecvFrom(int32_t fd, string& recv_info_str, bool need_closed) {
    char recv_buf[MetaSize];
    int32_t transfer_id;
    struct sockaddr_in from;
    socklen_t len = sizeof(from);

    do {
        memset(recv_buf, 0, sizeof(recv_buf));
        int32_t buf_len = recvfrom(fd, recv_buf, MetaSize, 0, (struct sockaddr *)&from, &len);
        if (-1 == buf_len && EAGAIN == errno) {
            /*
             * Resource temporarily unavailable! wait and recv again!
             */
            usleep(1);
            continue;
        } else if (buf_len < 0) {
            fprintf(stderr, "Call RpcRecv func error!, buf len is %d, the errno is %s.\n", buf_len, strerror(errno));
            transfer_id = ERROR_RECV;
            break;
        } else if (0 == buf_len) {
            transfer_id = 0;
            break;
        }
        MetaData* md_ptr = (MetaData*)recv_buf;
        recv_info_str.append(md_ptr->body, strlen(md_ptr->body));
        if (md_ptr->h_code < 0) {
            transfer_id = md_ptr->h_code * -1;
            break;
        }
    } while(true);

    if (need_closed) {
        close(fd);
    }

    return transfer_id;
}

int32_t RpcSendTo(int32_t fd, struct sockaddr_in *to, string& send_info_str, bool need_closed) {
    const char* send_ptr = send_info_str.c_str();
    int32_t block_num = send_info_str.size() / (BODY_SIZE - 1) + 1;

    MetaData meta_data;
    for (int i = 0; i < block_num - 1; ++i) {
        memset(&meta_data, 0, sizeof(meta_data));
        memcpy(meta_data.body, send_ptr + i * (BODY_SIZE - 1), BODY_SIZE - 1);
        (meta_data.body)[BODY_SIZE - 1] = '\0';
        meta_data.h_code = i;
        if (sendto(fd, &meta_data, MetaSize, 0, (struct sockaddr *)to, sizeof(to)) < 0) {
            fprintf(stderr, "Send Meta Data failed!, the errno is %s.\n", strerror(errno));
            close(fd);
            return -1;
        }
    }

    int32_t rest_len = send_info_str.size() % (BODY_SIZE - 1) ;
    if (0 != rest_len) {
        memset(&meta_data, 0, sizeof(meta_data));
        memcpy(meta_data.body, send_ptr + (block_num - 1)* (BODY_SIZE  - 1), rest_len);
        (meta_data.body)[rest_len] = '\0';
        meta_data.h_code = 1;
        if (sendto(fd, &meta_data, MetaSize, 0, (struct sockaddr *)to, sizeof(to)) < 0) {
            fprintf(stderr, "Send Meta Data failed!, the errno is %s.\n", strerror(errno));
            close(fd);
            return -1;
        }
    }

    if (need_closed) {
        close(fd);
    }
    return 0;
}

int32_t GetPeerAddr(int32_t fd, std::string& guest_addr) {
    sockaddr_in guest;
    socklen_t guest_len = sizeof(guest);
    if (getpeername(fd, (struct sockaddr *)&guest, &guest_len) < 0) {
        return -1;
    }
    char* addr = inet_ntoa(guest.sin_addr);
    if (NULL == addr || strlen(addr) == 0) {
        return -1;
    }
    guest_addr = addr;
    return 0;
}

}  // end of namespace libevrpc









/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
