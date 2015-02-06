/***************************************************************************
 * 
 * Copyright (c) 2014 aishuyu, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file ds_log.cpp
 * @author aishuyu(asy5178@163.com)
 * @date 2014/12/12 17:21:01
 * @brief 
 *  
 **/


#include "ds_log.h"

namespace dist_storage {

using namespace google;

#define INVALID_LEVEL -1

LogSeverity LogLevelMapping(const char* log_level) {
    if (strcmp("INFO", log_level) == 0) {
        return INFO;
    } else if (strcmp("WARNING", log_level) == 0) {
        return WARNING;
    } else if (strcmp("ERROR", log_level) == 0) {
        return ERROR;
    } else if (strcmp("FATAL", log_level) == 0) {
        return FATAL;
    }
    return INVALID_LEVEL;
}

bool DSLogInit(int argc, char* argv[], const char* log_level, const char* log_dir) {
    InitGoogleLogging(argv[0]);
    //google::ParseCommandLineFlags(&argc, &argv, true);
    int32_t level = LogLevelMapping(log_level);
    if (level == INVALID_LEVEL) {
        DS_LOG(ERROR, "DSLogInit failed! set invalid level!");
        return false;
    }
    FLAGS_logbuflevel = -1;
    FLAGS_log_dir = log_dir;
    FLAGS_minloglevel = level;
    return true;
}

bool DSLogShutDown() {
    google::ShutdownGoogleLogging();
}

}  // end of namespace dist_storage






/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
