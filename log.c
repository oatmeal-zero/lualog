/* File: log.c
 * Auth: linwx
 * Date: 2013/11/15
 * Lua日志模块
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/timeb.h>
#include <time.h>
#include <pthread.h>
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#ifdef USING_ZLOG
#include "zlog.h"

#define DZLOG_FATAL(file, line, msg) \
    dzlog(file, strlen(file), "unknown", 0, line, \
    ZLOG_LEVEL_FATAL, msg)

#define DZLOG_ERROR(file, line, msg) \
    dzlog(file, strlen(file), "unknown", 0, line, \
    ZLOG_LEVEL_ERROR, msg)

#define DZLOG_WARN(file, line, msg) \
    dzlog(file, strlen(file), "unknown", 0, line, \
    ZLOG_LEVEL_WARN, msg)

#define DZLOG_INFO(file, line, msg) \
    dzlog(file, strlen(file), "unknown", 0, line, \
    ZLOG_LEVEL_INFO, msg)

#define DZLOG_DEBUG(file, line, msg) \
    dzlog(file, strlen(file), "unknown", 0, line, \
    ZLOG_LEVEL_DEBUG, msg)
    
#else

enum
{
    NONE,
    FATAL,
    ERROR,
    WARN,
    INFO,
    DEBUG
};

int get_lua_log_level();
void init_lua_log_level();

/* 取文件名中去除路径部分的文件名 */
const char* get_file_name(const char* file)
{
    if (file != NULL && strlen(file) > 0)
    {
        const char *p = strrchr(file, '/');
        if (p != NULL)
        {
            return (p + 1);
        }
    }
    return file;
}

/* 打印日期时间 */
void print_time(FILE* fp, const char *File, int Line, char level)
{
    struct timeb tp;
    struct tm *ptm;
    ftime(&tp);
    ptm = localtime(&tp.time);
    fprintf(fp, "%02d/%02d %02d:%02d:%02d:%03d|P%uT%lu|%c|%s:%d|",
            ptm->tm_mon+1,
            ptm->tm_mday,
            ptm->tm_hour,
            ptm->tm_min,
            ptm->tm_sec,
            tp.millitm,
            getpid(), // 进程ID
            pthread_self(), // 线程ID
            level,
            get_file_name(File),
            Line);
    fflush(fp);
}

/* 打印日志基础方法 */
void log_base(lua_State* L, char level)
{
    int line = luaL_checkint(L, 2);
    print_time(stdout, lua_tostring(L, 1), line, level);
    fprintf(stdout, lua_tostring(L, 3));
}
#endif

/* 打印日志，供Lua调用 */
int l_log_fatal(lua_State* L)
{
#ifdef USING_ZLOG
    int line = luaL_checkint(L, 2);
    DZLOG_FATAL(lua_tostring(L, 1), line, lua_tostring(L, 3));
    return 0;
#else
    int printed = 0;
    if (get_lua_log_level() >= FATAL)
    {
        log_base(L, 'F');
        printed = 1;
    }
    lua_pushboolean(L, printed);
    return 1;
#endif    
}

int l_log_error(lua_State* L)
{
#ifdef USING_ZLOG
    int line = luaL_checkint(L, 2);
    DZLOG_ERROR(lua_tostring(L, 1), line, lua_tostring(L, 3));
    return 0;
#else
    int printed = 0;
    if (get_lua_log_level() >= ERROR)
    {
        log_base(L, 'E');
        printed = 1;
    }
    lua_pushboolean(L, printed);
    return 1;
#endif        
}

int l_log_warn(lua_State* L)
{
#ifdef USING_ZLOG
    int line = luaL_checkint(L, 2);
    DZLOG_WARN(lua_tostring(L, 1), line, lua_tostring(L, 3));
    return 0;
#else
    int printed = 0;
    if (get_lua_log_level() >= WARN)
    {
        log_base(L, 'W');
        printed = 1;
    }
    lua_pushboolean(L, printed);
    return 1;
#endif    
}

int l_log_info(lua_State* L)
{
#ifdef USING_ZLOG
    int line = luaL_checkint(L, 2);
    DZLOG_INFO(lua_tostring(L, 1), line, lua_tostring(L, 3));
    return 0;
#else
    int printed = 0;
    if (get_lua_log_level() >= INFO)
    {
        log_base(L, 'I');
        printed = 1;
    }
    lua_pushboolean(L, printed);
    return 1;
#endif    
}

int l_log_debug(lua_State* L)
{
#ifdef USING_ZLOG
    int line = luaL_checkint(L, 2);
    DZLOG_DEBUG(lua_tostring(L, 1), line, lua_tostring(L, 3));
    return 0;
#else
    int printed = 0;
    if (get_lua_log_level() >= DEBUG)
    {
        log_base(L, 'D');
        printed = 1;
    }
    lua_pushboolean(L, printed);
    return 1;
#endif    
}

int luaopen_lib_lualog(lua_State* L)
{
    static const luaL_Reg funcs[] = 
    {
        {"l_log_fatal", l_log_fatal},
        {"l_log_error", l_log_error},
        {"l_log_warn", l_log_warn},
        {"l_log_info", l_log_info},
        {"l_log_debug", l_log_debug},
        {NULL, NULL}
    };

    luaL_newlib(L, funcs);
#ifndef USING_ZLOG    
    init_lua_log_level();
#endif    
    return 1;
}
