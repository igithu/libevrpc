/***************************************************************************
 *
 * Copyright (c) 2016 aishuyu, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file consistent_hash_load_balancer.h
 * @author aishuyu(asy5178@163.com)
 * @date 2016/09/08 20:11:27
 * @brief
 *
 **/




#ifndef __CONSISTENT_HASH_LOAD_BALANCER_H
#define __CONSISTENT_HASH_LOAD_BALANCER_H

#include "load_balancer.h"

#include <memory>
#include <unordered_map>
#include <map>

namespace libevrpc {

/*
 * 虚拟节点, 对应多个实际RpcServer
 */
struct VirtualNode {
    float vn_load
    std::vector<std::string> py_node_list;
};

typedef std::shared_ptr<VirtualNode> VN_PTR;
typedef std::map<uint32_t, VN_PTR> VN_HASH_MAP;

class ConsistentHashLoadBalancer : public LoadBalancer {
    public:
        ConsistentHashLoadBalancer();
        virtual ~ConsistentHashLoadBalancer();

        bool InitBalancer();
        void SetConfigFile(const std::string& file_name)
        bool AddRpcServer(const std::string& rpc_server);
        void GetRpcServer(const std::string& rpc_client,
                          std::vector<string>& rpc_server_list);

    private:
        bool BuildConsistentHashMap();

    private:
        std::string config_file_;
        int32_t virtual_node_num_;

        VN_HASH_MAP* vn_map_ptr_;
};

}  // end of namespace





#endif // __CONSISTENT_HASH_LOAD_BALANCER_H



/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
