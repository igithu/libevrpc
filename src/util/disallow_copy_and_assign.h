/***************************************************************************
 * 
 * Copyright (c) 2014 aishuyu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file disallow_copy_and_assign.h
 * @author aishuyu(asy5178@163.com)
 * @date 2014/11/01 22:19:56
 * @brief 
 *  
 **/




#ifndef  __DISALLOW_COPY_AND_ASSIGN_H_
#define  __DISALLOW_COPY_AND_ASSIGN_H_




// A macro to disallow the copy constructor and operator= functions
// This should be used in the private: declarations for a class
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName&); \
    TypeName& operator=(const TypeName&);




#endif  //__DISALLOW_COPY_AND_ASSIGN_H_

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
