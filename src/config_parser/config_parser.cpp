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

namespace libevrpc {

using std::string;

ConfigParser::ConfigParser(const std::string& ini_file) :
    is_init_(false),
    ini_file_(ini_file),
    dict_ini_(NULL) {
        InitConfigParser();
}

ConfigParser::~ConfigParser() {
    if (NULL != dict_ini_) {
        iniparser_freedict(dict_ini_);
    }
}

// ConfigParser& ConfigParser::GetInstance() {
//     static ConfigParser cp_instance;
//     return cp_instance;
// }

ConfigParser& ConfigParser::GetInstance(const std::string& ini_file) {
    static ConfigParser cp_instance(ini_file);
    return cp_instance;
}

bool ConfigParser::InitConfigParser() {
    dict_ini_ = iniparser_load(ini_file_.c_str());
    return true;
}

int32_t ConfigParser::GetSecKeysNum(const char* sec_key, int32_t defalut_val) {
    if (NULL == dict_ini_) {
        return defalut_val;
    }
    return iniparser_getsecnkeys(dict_ini_, const_cast<char*>(sec_key));
}

int32_t ConfigParser::IniGetInt(const char* sec_key, int32_t defalut_val) {
    if (NULL == dict_ini_) {
        return defalut_val;
    }
    return iniparser_getint(dict_ini_, sec_key, defalut_val);
}

double ConfigParser::IniGetDouble(const char* sec_key, double defalut_val) {
    if (NULL == dict_ini_) {
        return defalut_val;
    }
    return iniparser_getdouble(dict_ini_, sec_key, defalut_val);
}

const char* ConfigParser::IniGetString(const char* sec_key, const char* defalut_val) {
    if (NULL == dict_ini_) {
        defalut_val;
    }
    return iniparser_getstring(dict_ini_, sec_key, const_cast<char*>(defalut_val));
}

bool ConfigParser::IniGetBool(const char* sec_key, bool defalut_val) {
    if (NULL == dict_ini_) {
        defalut_val;
    }
    return iniparser_getboolean(dict_ini_, sec_key, defalut_val);;
}


}  // end of namespace libevrpc





/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
