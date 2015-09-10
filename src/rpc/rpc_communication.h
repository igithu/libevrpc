/***************************************************************************
 *
 * Copyright (c) 2015 aishuyu, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file rpc_commonication.h
 * @author aishuyu(asy5178@163.com)
 * @date 2015/09/03 18:22:08
 * @brief
 *
 **/




#ifndef __RPC_COMMONICATION_H
#define __RPC_COMMONICATION_H


#include <sys/socket.h>
#include <netinet/in.h>

#include <string>

namespace libevrpc {

// #define SER_INTERNAL_ERROR 500
// #define SER_RETURN_SUCC    200
#define ERROR_META -300
#define ERROR_SEND -500
#define ERROR_RECV -200

#define MAX_INFO_LEN 512
#define BODY_SIZE 16

typedef struct MetaDataStruct {
    int32_t h_code;
    char body[BODY_SIZE];
} MetaData;

const static int32_t MetaSize = sizeof(MetaData);


//  wrapper socket, imitate the <Unix socket> code
//  for TcpListen Accept api have the flag mark non_block, the non_block is true for test,
//  default false for the rpc
int32_t Socket(int32_t family = 0, int32_t type = 0, int32_t protocol = 0);

int32_t TcpListen(const char *host, const char *port, bool non_block = true, int32_t family = AF_UNSPEC);

int32_t TcpConnect(const char *host, const char *port, int32_t family = AF_UNSPEC);

int32_t Accept(int fd, struct sockaddr_in& sa, int32_t addrlen, bool non_block = true);

int32_t RpcRecv(int32_t fd, std::string& recv_info_str, bool need_closed = false);

int32_t RpcSend(int32_t fd, int32_t transfer_id, std::string& send_info_str, bool need_closed = false);

bool SetNonBlock(int32_t sock);

}  // end of namespace libevrpc



#endif // __RPC_COMMONICATION_H



/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
