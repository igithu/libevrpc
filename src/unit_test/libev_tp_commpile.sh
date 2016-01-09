#!/bin/sh

UTIL_PATH=../util
SERVER_PATH=../rpc_server

g++ -o ut_libev_tp test_libev_tp.cpp $UTIL_PATH/pthread_mutex.h $SERVER_PATH/libev_thread_pool.h $SERVER_PATH/libev_thread_pool.cpp -lev

