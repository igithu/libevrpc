/***************************************************************************
 *
 * Copyright (c) 2016 aishuyu, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file rpc_center.h
 * @author aishuyu(asy5178@163.com)
 * @date 2016/07/12 20:33:03
 * @brief
 *
 **/




#ifndef __RPC_CENTER_H
#define __RPC_CENTER_H

namespace libevrpc {

class RpcCenter {
    public:
        ~RpcCenter();

        static RpcCenter& GetInstance();
    private:
        RpcCenter();
};


}  // end of namespace



#endif // __RPC_CENTER_H



/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
