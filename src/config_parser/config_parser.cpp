/***************************************************************************
 *
 * Copyright (c) 2015 aishuyu, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file config_parser.cpp
 * @author aishuyu(asy5178@163.com)
 * @date 2016/04/06 20:12:14
 * @brief
 *
 **/

#include "config_parser.h"

namespace libevepc {

using std::string;

ConfigParser::ConfigParser() :
    is_init_(false),
    ini_file_("../conf/libevrpc.ini"),
    dict_ini_(NULL) {
}

ConfigParser::ConfigParser(const std::string& ini_file) :
    is_init_(false),
    ini_file_(ini_file),
    dict_ini_(NULL) {
}

ConfigParser::~ConfigParser() {
}

ConfigParser& ConfigParser::GetInstance() {
    static ConfigParser cp_instance;
    return cp_instance;
}

ConfigParser& ConfigParser::GetInstance(const std::string& ini_file) {
    static ConfigParser cp_instance(ini_file);
    return cp_instance;
}

bool ConfigParser::InitConfigParser() {
    return true;
}

int32_t ConfigParser::GetSecKeysNum(const string& sec_key) {
}

int32_t ConfigParser::IniGetInt(const string& sec_key) {
}

double ConfigParser::IniGetDouble(const string& sec_key) {
}

const char* ConfigParser::IniGetString(const string& sec_key) {
}

bool ConfigParser::IniGetBool(const string& sec_key) {
    return true;
}




}  // end of namespace libevepc





/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
