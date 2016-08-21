/***************************************************************************
 *
 * Copyright (c) 2016 aishuyu, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file rpc_center_server.cc
 * @author aishuyu(asy5178@163.com)
 * @date 2016/08/19 20:46:12
 * @brief
 *
 **/


#include "rpc_center.h"

#include <signal.h>
#include <stdio.h>
#include <fcntl.h>


using namespace libevrpc;

inline void InitSignal() {
    struct sigaction sa;
    memset(&sa, 0, sizeof(struct sigaction));

    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    if (-1 == sigaction(SIGPIPE, &sa, NULL)) {
        exit(1);
    }
}

inline void Daemonize() {
    if (0 != fork()) {
        exit(0);
    }
    if (-1 == setsid()) {
        fprintf(stderr, "set sid error!\n");
        exit(-1);
    }
    umask(0);
    int fd = open("/dev/null", O_RDWR, 0);
    if (-1 != fd) {
        dup2(fd, STDIN_FILENO);
        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);
        if (fd > STDERR_FILENO) {
            close(fd);
        }
    }
}

int main(int argc, char* argv[]) {
    InitSignal();
    if (true) {
        Daemonize();
    }
    RpcCenter& rc = RpcCenter::GetInstance("conf/rpc_center.ini");
    rc.StartCenter();
    rc.WaitCenter();
    rc.StopCenter();
    return 0;
}








/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
