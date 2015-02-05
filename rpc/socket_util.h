/***************************************************************************
 * 
 * Copyright (c) 2015 aishuyu, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file socket_util.h
 * @author aishuyu(asy5178@163.com)
 * @date 2015/01/28 14:31:42
 * @brief 
 *  
 **/




#ifndef  __SOCKET_UTIL_H_
#define  __SOCKET_UTIL_H_

#include <sys/socket.h>
#include <netinet/in.h>

#include <string>

namespace libevrpc {

//  wrapper socket, imitate the <Unix socket> code
int32_t Socket(int32_t family = 0, int32_t type = 0, int32_t protocol = 0);

int32_t TcpListen(const char *host, const char *port, int32_t family = AF_UNSPEC);

int32_t TcpConnect(const char *host, const char *port, int32_t family = AF_UNSPEC);

int32_t Accept(int fd, struct sockaddr_in& sa, int32_t addrlen);

int32_t RecvMsg(int32_t fd, std::string& recv_msg_str);

int32_t SendMsg(int32_t fd, std::string& send_msg_str);

bool SetNonBlock(int32_t sock);

}  // end of namespace libevrpc



#endif  //__SOCKET_UTIL_H_

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
