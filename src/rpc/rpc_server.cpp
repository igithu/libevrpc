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
#include <exception>

#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/stubs/common.h>

#include "rpc_util.h"
#include "rpc_communication.h"

namespace libevrpc {

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

// Registe the service into map
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

    printf("Start wait for io .....\n");
    if (NULL != io_thread_ptr_) {
        io_thread_ptr_->Wait();
    }

    printf("Start wait for worker .....\n");
    if (NULL != worker_threads_ptr_) {
        worker_threads_ptr_->Wait();
    }

    printf("Start wait for reader .....\n");
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

    if (NULL != reader_threads_ptr_) {
        reader_threads_ptr_->Processor(RpcServer::RpcReader, cb_params_ptr);
    } else {
        worker_threads_ptr_->Processor(RpcServer::RpcProcessor, cb_params_ptr);
    }
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
        delete cb_params_ptr;
        cb_params_ptr = NULL;
        return NULL;
    }
    int32_t event_fd = cb_params_ptr->event_fd;

    // start recv the msg
    string recv_info;
    int32_t call_id = -1;
    if (NULL == rpc_serv_ptr->reader_threads_ptr_) {
        call_id = RpcRecv(event_fd, recv_info, false);
        if (ERROR_RECV == call_id) {
            perror("Recv data in RpcProcessor error!");
            delete cb_params_ptr;
            cb_params_ptr = NULL;
            close(event_fd);
            return NULL;
        }
    }

    // find the function will be called
    HashMap& method_hashmap = rpc_serv_ptr->method_hashmap_;
    HashMap::iterator method_iter = method_hashmap.find(call_id);
    if (method_iter == method_hashmap.end() || NULL == method_iter->second) {
        perror("Find the method failed!");
        delete cb_params_ptr;
        cb_params_ptr = NULL;
        close(event_fd);
        return NULL;
    }

    RpcMethod* rpc_method = method_iter->second;
    Message* request = rpc_method->request->New();
    if (!request->ParseFromString(recv_info)) {
        perror("Parse body msg error!");
        delete cb_params_ptr;
        delete request;
        cb_params_ptr = NULL;
        close(event_fd);
        return NULL;
    }

    const MethodDescriptor* method_desc = rpc_method->method;
    Message* response = rpc_method->response->New();
    // call method!!
    rpc_method->service->CallMethod(method_desc, NULL, request, response, NULL);

    // get send info
    string response_str = "";
    if (!response->SerializeToString(&response_str)) {
        perror("SerializeToString response failed!");
        delete cb_params_ptr;
        delete request;
        delete response;
        cb_params_ptr = NULL;
        close(event_fd);
        return NULL;
    }

    if (NULL == rpc_serv_ptr->writer_threads_ptr_) {
        /*The writer pool is not started*/
        if (RpcSend(event_fd, 0, response_str, true) < 0) {
            perror("Send info data in RpcProcessor failed!");
        }
    } else {
        /*The writer pool is started, push the task to writer pool */
        cb_params_ptr->response_ptr = response;
        rpc_serv_ptr->writer_threads_ptr_->Processor(
            RpcServer::RpcWriter, cb_params_ptr);
    }

}

void* RpcServer::RpcReader(void *arg) {

    CallBackParams* cb_params_ptr = (CallBackParams*) arg;
    if (NULL == cb_params_ptr) {
        return NULL;
    }

    RpcServer* rpc_serv_ptr = cb_params_ptr->rpc_server_ptr;
    if (NULL == rpc_serv_ptr) {
        delete cb_params_ptr;
    }
    int32_t event_fd = cb_params_ptr->event_fd;

    cb_params_ptr->call_id = RpcRecv(event_fd, cb_params_ptr->recv_info, false);

    // push the task into processor
    rpc_serv_ptr->worker_threads_ptr_->Processor(
            RpcServer::RpcProcessor, cb_params_ptr);
}

void* RpcServer::RpcWriter(void *arg) {
    CallBackParams* cb_params_ptr = (CallBackParams*) arg;
    if (NULL == cb_params_ptr) {
        return NULL;
    }

    RpcServer* rpc_serv_ptr = cb_params_ptr->rpc_server_ptr;
    if (NULL == rpc_serv_ptr) {
        delete cb_params_ptr;
        return NULL;
    }
    int32_t event_fd = cb_params_ptr->event_fd;

    string response_str;
    if (!cb_params_ptr->response_ptr->SerializeToString(&response_str)) {
        perror("SerializeToString response failed!");
    }

    if (RpcSend(event_fd, 0, response_str, true) < 0) {
        perror("Send the info data failed!");
    }
    // The current cb_params_ptr never be used!
    delete cb_params_ptr;
}

}  // end of namespace libevrpc





/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
