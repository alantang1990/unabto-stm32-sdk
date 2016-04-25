/*
* Copyright (C) 2008-2013 Nabto - All Rights Reserved.
*/
#ifndef _UNABTO_PLATFORM_H_
#define _UNABTO_PLATFORM_H_

#include <unabto/unabto_util.h>
#include "unabto_platform_types.h"
#include <platforms/unabto_common_types.h>

/**
* Generic typedefs
*/

typedef int ssize_t;

/**
* Socket related definitions
*/

#define NABTO_INVALID_SOCKET -1

/**
* Time related definitions
*/

#define nabtoGetStamp(void) HAL_GetTick()
#define nabtoMsec2Stamp

/**
* Logging related definitions
*/

#include <stdarg.h>

#ifdef __cplusplus
 extern "C" {
#endif
void p(const char *fmt, ... );
#ifdef __cplusplus
}
#endif

#define NABTO_LOG_BASIC_PRINT(severity, msg) p msg;

#endif

