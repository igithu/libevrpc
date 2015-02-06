/***************************************************************************
 * 
 * Copyright (c) 2014 aishuyu, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file ds_log.h
 * @author aishuyu(asy5178@163.com)
 * @date 2014/12/12 10:13:38
 * @brief you can replace the current log tool with yours
 *  
 **/




#ifndef  __DS_LOG_H_
#define  __DS_LOG_H_

#include <glog/logging.h>

#define MAX_BUF 256

// #define DS_LOG(severity) LOG(severity)
#define DS_LOG(_loglevel_, _fmt_, ...)  { char buf[MAX_BUF];  snprintf(buf, MAX_BUF, _fmt_, ##__VA_ARGS__);  LOG(_loglevel_) << buf << "\n" ;}

namespace dist_storage {

bool DSLogInit(
        int argc, char* argv[], const char* log_level, const char* log_dir);

bool DSLogShutDown() ;

}  // end of namespace dist_storage

#endif  //__DS_LOG_H_

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
