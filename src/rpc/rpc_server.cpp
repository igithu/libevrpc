/***************************************************************************
 * 
 * Copyright (c) 2014 Aishuyu. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file rpc_server.cpp
 * @author aishuyu(asy5178@163.com)
 * @date 2014/11/24 00:10:22
 * @brief 
 *  
 **/

#include "rpc_server.h"

#include <string>

#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/stubs/common.h>

#include "socket_util.h"
#include "rpc_util.h"
#include "libevrpc_log.h"

namespace libevrpc {

//using std::string;

#define MAXEVENTS 100

RpcServer::RpcServer() :
    libev_connector_ptr_(NULL),
    io_thread_ptr_(NULL),
    worker_threads_ptr_(NULL),
    reader_threads_ptr_(NULL), 
    writer_threads_ptr_(NULL) {
    Initialize();
}

RpcServer::~RpcServer() {
    for (HashMap::iterator iter = method_hashmap_.begin(); 
         iter != method_hashmap_.end();
         ++iter) {
        RpcMethod* rpc_method_ptr = iter->second;
        if (NULL != rpc_method_ptr) {
            delete rpc_method_ptr;
            rpc_method_ptr = NULL;
        }
    }

    if (NULL != reader_threads_ptr_) {
        delete reader_threads_ptr_;
    }

    if (NULL != writer_threads_ptr_) {
        delete writer_threads_ptr_;
    }

    if (NULL != worker_threads_ptr_) {
        delete worker_threads_ptr_;
    }

    if (NULL != io_thread_ptr_) {
        delete io_thread_ptr_;
    }

    if (NULL != libev_connector_ptr_) {
        delete libev_connector_ptr_;
    }
}

// initial prameters
bool RpcServer::Initialize() {
    // should read from config file
    // strcpy(host_, "127.0.0.1");
    // strcpy(port_, "9998");

    return true;
}

// Note: not thread safe in old c++
RpcServer& RpcServer::GetInstance() {
    static RpcServer server_instance;
    return server_instance;
}

// Registe the serverice into map
bool RpcServer::RegisteService(Service* reg_service) {
    const ServiceDescriptor* descriptor = reg_service->GetDescriptor();
    for (int32_t i = 0; i < descriptor->method_count(); ++i) {
        const MethodDescriptor* method_desc = descriptor->method(i);
        const Message* request = &reg_service->GetRequestPrototype(method_desc);
        const Message* response = &reg_service->GetResponsePrototype(method_desc);

        // format rpc method
        RpcMethod* rpc_method =
            new RpcMethod(reg_service, request, response, method_desc);

        // format hash code of function called
        uint32_t hash_code = BKDRHash(method_desc->full_name().c_str());

        HashMap::iterator ret_iter = method_hashmap_.find(hash_code);
        if (ret_iter == method_hashmap_.end()) {
            method_hashmap_.insert(std::make_pair(hash_code, rpc_method));
        } else {
            // if conflict, replace old one
            delete ret_iter->second;
            method_hashmap_[hash_code] = rpc_method;
        }
    }
   return true;
}

bool RpcServer::Start(const char* addr,
                      const char* port,
                      int32_t thread_num,
                      int32_t reader_num,
                      int32_t writer_num) {

    LIBEVRPC_LOG(INFO, "rpc server start info, thread pool num: %d, addr: %s, port: %s", 
            thread_num, addr, port);

    libev_connector_ptr_ = new LibevConnector();
    io_thread_ptr_ = new IOThread(addr, port);
    worker_threads_ptr_ = new ThreadPool(thread_num);

    io_thread_ptr_->Start();
    worker_threads_ptr_->Start();

    // if start readerpool or writerpool
    if (0 != reader_num) {
        reader_threads_ptr_ = new ThreadPool(reader_num);
        reader_threads_ptr_->Start();
    }

    if (0 != writer_num) {
        writer_threads_ptr_ = new ThreadPool(writer_num);
        writer_threads_ptr_->Start();
    }
}

bool RpcServer::Wait() {
    if (false == io_thread_ptr_->IsAlive()) {
        //worker_threads_ptr_->Destroy();
        return false;
    }

    if (NULL != io_thread_ptr_) {
        io_thread_ptr_->Wait();
    }

    if (NULL != worker_threads_ptr_) {
        worker_threads_ptr_->Wait();
    }

    if (NULL != reader_threads_ptr_) {
        reader_threads_ptr_->Wait();
    }

    if (NULL != writer_threads_ptr_) {
        writer_threads_ptr_->Wait();
    }

    return true;
}

bool RpcServer::RpcCall(int32_t event_fd) {
    if (NULL == worker_threads_ptr_) {
        return false;
    }

    CallBackParams* cb_params_ptr = new CallBackParams();
    cb_params_ptr->event_fd = event_fd;
    cb_params_ptr->rpc_server_ptr = this;
    // push the task to thread pool
    worker_threads_ptr_->Processor(RpcServer::RpcProcessor, cb_params_ptr);
    return true;
}

LibevConnector* RpcServer::GetLibevConnector() {
    return libev_connector_ptr_;
}

void* RpcServer::RpcProcessor(void *arg) {

    CallBackParams* cb_params_ptr = (CallBackParams*) arg;
    if (NULL == cb_params_ptr) {
        return NULL;
    }
    RpcServer* rpc_serv_ptr = cb_params_ptr->rpc_server_ptr;
    if (NULL == rpc_serv_ptr) {
        return NULL;
    }
    int32_t event_fd = cb_params_ptr->event_fd;

    RpcMessage recv_rpc_msg;
    if (!rpc_serv_ptr->GetMethodRequest(event_fd, recv_rpc_msg)) {
        //rpc_serv_ptr->ErrorSendMsg(event_fd, "get method request failed!");
        return NULL;
    }

    uint32_t hash_code = recv_rpc_msg.head_code();
    HashMap& method_hashmap = rpc_serv_ptr->method_hashmap_;
    // find the function will be called
    HashMap::iterator method_iter = method_hashmap.find(hash_code);
    if (method_iter == method_hashmap.end() || NULL == method_iter->second) {
        LIBEVRPC_LOG(ERROR, "find hash code failed! request hash code is: %u", hash_code);
        rpc_serv_ptr->ErrorSendMsg(event_fd, "find hash code failed!");
        return NULL;
    }
    RpcMethod* rpc_method = method_iter->second;
    Message* request = rpc_method->request->New();
    if ("0" != recv_rpc_msg.body_msg() &&
         !request->ParseFromString(recv_rpc_msg.body_msg())) {
        LIBEVRPC_LOG(ERROR, "parse body msg error!");
        rpc_serv_ptr->ErrorSendMsg(event_fd, "parse body msg error!");
        delete request;
        return NULL;
    }

    const MethodDescriptor* method_desc = rpc_method->method;
    Message* response = rpc_method->response->New();
    // call method!
    rpc_method->service->CallMethod(method_desc, NULL, request, response, NULL);

    if (!rpc_serv_ptr->SendFormatStringMsg(event_fd, response)) {
        LIBEVRPC_LOG(ERROR, "send format response failed!");
        rpc_serv_ptr->ErrorSendMsg(event_fd, "send format response failed!");
    }
    delete request;
    delete response;
    delete cb_params_ptr;
}


bool RpcServer::GetMethodRequest(int32_t event_fd, RpcMessage& recv_rpc_msg) {
    string msg_str;
    if (RecvMsg(event_fd, msg_str) < 0) {
        LIBEVRPC_LOG(ERROR, "rpc server recv msg failed!");
        return false;
    }

    if (0 == msg_str.size()) {
        close(event_fd);
        return false;
    }

    if (!recv_rpc_msg.ParseFromString(msg_str)) {
        LIBEVRPC_LOG(ERROR, "parse from string msg failed!");
        close(event_fd);
        return false;
    }
    return true;
}

bool RpcServer::SendFormatStringMsg(int32_t event_fd, Message* response) {
    string response_str;
    if (!response->SerializeToString(&response_str)) {
        LIBEVRPC_LOG(ERROR, "response_str SerializeToString failed!");
        return false;
    }
    if (0 == response_str.size()) {
        response_str = "0";
    }

    RpcMessage send_rpc_msg;
    send_rpc_msg.set_head_code(200);
    send_rpc_msg.set_body_msg(response_str);
    string send_str;
    if (!send_rpc_msg.SerializeToString(&send_str)) {
        LIBEVRPC_LOG(ERROR, "send_str SerializeToString failed!");
        return false;
    }
    SendMsg(event_fd, send_str);
    close(event_fd);
    //fsync(event_fd);
    //close(event_fd);
    //fflush(event_fd);


    return true;
}

bool RpcServer::ErrorSendMsg(int32_t event_fd, const string& error_msg) {
    RpcMessage error_rpc_msg;
    // 500 means internal error
    error_rpc_msg.set_head_code(500);
    error_rpc_msg.set_body_msg(error_msg);

    string err_msg_str;
    if (!error_rpc_msg.SerializeToString(&err_msg_str)) {
        LIBEVRPC_LOG(ERROR, "error send error!");
        close(event_fd);
        return false;
    }
    SendMsg(event_fd, err_msg_str);
    close(event_fd);
    return true;
}

}  // end of namespace libevrpc





/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
