/***************************************************************************
 *
 * Copyright (c) 2017 aishuyu, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file center_monitor.h
 * @author aishuyu(asy5178@163.com)
 * @date 2017/06/12 20:41:36
 * @brief
 *
 **/




#ifndef __CENTER_MONITOR_H
#define __CENTER_MONITOR_H


#include <string>

#include "util/disallow_copy_and_assign.h"

namespace libevrpc {

/*
 * For avoiding split-brain
 */
class CenterMonitor {
    public:
        ~CenterMonitor();

        static CenterMonitor& GetInstance(const std::string& config_file);

    private:
        CenterMonitor(const std::string& config_file);

        DISALLOW_COPY_AND_ASSIGN(CenterMonitor);
};


}  // end of namespace


#endif // __CENTER_MONITOR_H



/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
