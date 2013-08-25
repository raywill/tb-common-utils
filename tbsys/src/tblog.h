/*
 * (C) 2007-2010 Taobao Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *
 * Version: $Id$
 *
 * Authors:
 *   duolong <duolong@taobao.com>
 *
 */

#ifndef TBSYS_LOG_H
#define TBSYS_LOG_H

#include <stdarg.h>
#include <time.h>
#include <stdio.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <deque>
#include <string>
#include <pthread.h>
#include <sys/time.h>

#define TBSYS_LOG_LEVEL_ERROR 0
#define TBSYS_LOG_LEVEL_WARN  1
#define TBSYS_LOG_LEVEL_INFO  2
#define TBSYS_LOG_LEVEL_DEBUG 3
#define TBSYS_LOG_LEVEL(level) TBSYS_LOG_LEVEL_##level, __FILE__, __LINE__, __FUNCTION__
#define TBSYS_LOG_NUM_LEVEL(level) level, __FILE__, __LINE__, __FUNCTION__
#define TBSYS_LOGGER tbsys::CLogger::_logger
#define TBSYS_PRINT(level, ...) TBSYS_LOGGER.logMessage(TBSYS_LOG_LEVEL(level), __VA_ARGS__)
#define TBSYS_LOG_BASE(level, ...) (TBSYS_LOG_LEVEL_##level>TBSYS_LOGGER._level) ? (void)0 : TBSYS_PRINT(level, __VA_ARGS__) 
#define TBSYS_LOG(level, _fmt_, args...) ((TBSYS_LOG_LEVEL_##level>TBSYS_LOGGER._level) ? (void)0 : TBSYS_LOG_BASE(level, "[%ld] " _fmt_, pthread_self(), ##args))
#define TBSYS_LOG_US(level, _fmt_, args...) \
  ((TBSYS_LOG_LEVEL_##level>TBSYS_LOGGER._level) ? (void)0 : TBSYS_LOG_BASE(level, "[%ld][%ld][%ld] " _fmt_, \
                                                            pthread_self(), tbsys::CLogger::get_cur_tv().tv_sec, \
                                                            tbsys::CLogger::get_cur_tv().tv_usec, ##args))

namespace tbsys {
using std::deque;
using std::string;

/** 
* @brief 简单的日志系统 
*/
class           CLogger {
public:
    CLogger();
    ~CLogger();
    /** 
     * @brief 
     * 
     * @param filename
     * @param fmt
     */
    void rotateLog(const char *filename, const char *fmt = NULL);
    /** 
     * @brief 将日志内容写入文件
     * 
     * @param level 日志的级别
     * @param file  日志内容所在的文件
     * @param line  日志内容所在的文件的行号
     * @param function 写入日志内容的函数名称
     * @param fmt
     * @param ...
     */
    void logMessage(int level, const char *file, int line, const char *function, const char *fmt, ...);
    /** 
     * @brief 设置日志的级别
     * 
     * @param level DEBUG|WARN|INFO|ERROR
     */
    void setLogLevel(const char *level);
    /** 
     * @brief 设置日志文件的名称
     * 
     * @param filename 日志文件的名称
     */
    void setFileName(const char *filename, bool flag = false);
    /** 
     * @brief 检测文件是否已经打开,标准输出,错误输出重定向
     */
    void checkFile();
    void setCheck(int v) {_check = v;}
    /** 
     * @brief 设置日志文件文件的大小,达到maxFileSize就新打开一个文件
     * 如果不设置此项，日志系统会忽略日志滚动
     * 
     * @param maxFileSize 日志文件的大小
     */
    void setMaxFileSize( int64_t maxFileSize=0x40000000);
    /** 
     * @brief 保留最近maxFileIndex个日志文件，超出maxFileIndex个日志文件
     * 会按时间先后删除,但进程重启后日志系统会按时间先后重新统计
     * 
     * @param maxFileIndex 保留文件的最大个数
     */
    void setMaxFileIndex( int maxFileIndex= 0x0F);

    static inline struct timeval get_cur_tv()
    {
      struct timeval tv;
      gettimeofday(&tv, NULL);
      return tv;
    };

private:
    int _fd;
    char *_name;
    int _check;
    size_t _maxFileIndex;
    int64_t _maxFileSize;
    bool _flag;

public:
    static CLogger _logger;
    int _level;

private:
    std::deque<std::string> _fileList;
    static const char *const _errstr[];   
    pthread_mutex_t _fileSizeMutex;
    pthread_mutex_t _fileIndexMutex; 
};

}
#endif
