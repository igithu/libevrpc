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

#include "util/disallow_copy_and_assign.h"

namespace libevepc {

#define DEFAULT_INT -1
#define DEFAULT_DOUBLE -1.0
#define DEFAULT_CHAR NULL
#define DEFAULT_BOOL false

class ConfigParser {
    public:
        ~ConfigParser();

        static ConfigParser& GetInstance();
        static ConfigParser& GetInstance(const std::string& ini_file);

        int32_t GetSecKeysNum(const std::string& sec_key);
        int32_t IniGetInt(const std::string& sec_key);
        double IniGetDouble(const std::string& sec_key);
        const char *IniGetString(const std::string& sec_key);
        bool IniGetBool(const std::string& sec_key);
        // const char *IniGetLocalIPAddr();
        // const char *IniGetLocalHostName();

        void PrintErrorInfo();

    private:
        ConfigParser();
        ConfigParser(const std::string& ini_file);

        bool InitConfigParser();

        DISALLOW_COPY_AND_ASSIGN(ConfigParser);

    private:
        bool is_init_;
        std::string ini_file_;
        std::string error_info_;

        dictionary* dict_ini_;
};

}  // end of namespace libevepc


#endif // __CONFIG_PARSER_H



/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
