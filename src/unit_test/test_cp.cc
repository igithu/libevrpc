/***************************************************************************
 *
 * Copyright (c) 2016 aishuyu, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file test_cp.cc
 * @author aishuyu(asy5178@163.com)
 * @date 2016/07/12 00:06:36
 * @brief
 *
 **/




#include "../config_parser/config_parser.h"


using namespace libevrpc;

int main() {
    ConfigParser& cp = ConfigParser::GetInstance("test.ini");

    const char* test1 = cp.IniGetString("test1", "test-1-test");

    printf("The having default is %s\n", test1);

    return 0;
}








/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
