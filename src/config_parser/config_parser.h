/***************************************************************************
 *
 * Copyright (c) 2015 aishuyu, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file config_parser.h
 * @author aishuyu(asy5178@163.com)
 * @date 2016/04/06 19:48:07
 * @brief
 *
 **/




#ifndef __CONFIG_PARSER_H
#define __CONFIG_PARSER_H

#include <string>

#include "iniparser.h"
#include "util/disallow_copy_and_assign.h"

namespace libevrpc {

#define DEFAULT_INT -1
#define DEFAULT_DOUBLE -1.0
#define DEFAULT_CHAR NULL
#define DEFAULT_BOOL false

class ConfigParser {
    public:
        ~ConfigParser();

        // static ConfigParser& GetInstance();
        static ConfigParser& GetInstance(const std::string& ini_file);

        int32_t GetSecKeysNum(const char* sec_key, int32_t defalut_val);
        int32_t IniGetInt(const char* sec_key, int32_t defalut_val);
        double IniGetDouble(const char* sec_key, double defalut_val);
        const char *IniGetString(const char* sec_key, const char* defalut_val);
        bool IniGetBool(const char* sec_key, bool defalut_val);
        // const char *IniGetLocalIPAddr();
        // const char *IniGetLocalHostName();

        void PrintErrorInfo();

    private:
        // ConfigParser();
        ConfigParser(const std::string& ini_file);

        bool InitConfigParser();

        DISALLOW_COPY_AND_ASSIGN(ConfigParser);

    private:
        bool is_init_;
        std::string ini_file_;
        std::string error_info_;

        dictionary* dict_ini_;
};

}  // end of namespace libevrpc


#endif // __CONFIG_PARSER_H



/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
