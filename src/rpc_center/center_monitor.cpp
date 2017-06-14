/***************************************************************************
 *
 * Copyright (c) 2017 aishuyu, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file center_monitor.cpp
 * @author aishuyu(asy5178@163.com)
 * @date 2017/06/12 20:48:42
 * @brief
 *
 **/


#include "center_monitor.h"


namespace libevrpc {

using std::string;

CenterMonitor::CenterMonitor(const string& config_file) {
}

CenterMonitor::~CenterMonitor() {
}

CenterMonitor& CenterMonitor::GetInstance(const string& config_file) {
    static CenterMonitor cm_instance(config_file);
    return cm_instance;
}



}  // end of namespace





/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
