#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include <string>
#include <string.h>

#include <test_def.h>
#include <../rpc_communication.h>

using namespace libevrpc;

int main() {
    int32_t listen_fd = TcpListen(ADDR, PORT, false);

    struct sockaddr_in client_addr;
    socklen_t len = sizeof(struct sockaddr_in);
    printf("Test the listen_fd is : %d\n", listen_fd);
    int32_t cfd = Accept(listen_fd, client_addr, len, false);
    if (cfd < 0) {
        printf("accpet eror!\n");
        return 0;
    }

    std::string  recv_msg;
    int id = RpcRecv(cfd, recv_msg, true);

    printf("Test string is :%s\n", recv_msg.c_str());

    return 0;
}

