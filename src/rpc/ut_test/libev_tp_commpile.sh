#!/bin/sh

g++ -o ut_libev_tp test_libev_tp.cpp ../pthread_mutex.h ../libev_thread_pool.h ../libev_thread_pool.cpp -lev

