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
#include <vector>

#include "center_proto/centers.pb.h"
#include "center_proto/center_cluster.pb.h"

namespace libevrpc {

class LoadBalancer {
    public:
        virtual ~LoadBalancer() {}

        virtual bool InitBalancer() = 0;
        virtual void SetConfigFile(const std::string& file_name) = 0;
        virtual bool AddRpcServer(const RpcClusterServer& rpc_cluster_server) = 0;
        virtual bool GetRpcServer(const std::string& rpc_client, std::vector<std::string>& rpc_server_list) = 0;
        virtual bool GetCurrentLBResult(::google::protobuf::RepeatedPtrField<LoadBalancerMetaData>& lb_result_list) = 0;
        virtual bool UpdateLBResult(const ::google::protobuf::RepeatedPtrField<LoadBalancerMetaData>& lb_result_list) = 0;


};  // end of namespace libevrpc


}  // end of namespace libevrpc





#endif // __LOAD_BALANCER_H



/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
