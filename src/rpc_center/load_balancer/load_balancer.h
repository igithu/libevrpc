/***************************************************************************
 *
 * Copyright (c) 2016 aishuyu, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file load_balancer.h
 * @author aishuyu(asy5178@163.com)
 * @date 2016/07/12 20:40:20
 * @brief
 *
 **/




#ifndef __LOAD_BALANCER_H
#define __LOAD_BALANCER_H

#include <string>

namespace libevrpc {

class LoadBalancer {
    public:
        LoadBalancer(const std::string& config_file);
        virtual ~LoadBalancer();

        virtual bool InitBalancer() = 0;
        virtual bool AddRpcServer(const std::string& rpc_server) = 0;
        virtual std::string GetRpcServer(const std::string& rpc_client) = 0;

    private:

};  // end of namespace libevrpc


}  // end of namespace libevrpc





#endif // __LOAD_BALANCER_H



/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
