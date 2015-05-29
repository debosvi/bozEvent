/*
The MIT License (MIT)

Copyright (c) 2013 Vincent de RIBOU <belzo2005-dolphin@yahoo.fr>

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/*!
 * \file        bevt_client_p.h
 * \brief       Message Management private APIs.
 * \version     0.1
 * \date        2013/01/14
 * \author      Vincent de RIBOU.
 * \copyright   Aquaplouf Land.
 *
 */

#ifndef _BEVT_DEBUG_PRIV_H_
#define _BEVT_DEBUG_PRIV_H_

/**
 * \ingroup BOZCORE_PRIV
 * \defgroup BOZCONNECT_PRIV Message Management private APIs.
 * \{
 */

#ifdef __cplusplus
extern "C"
{
#endif

#include <skalibs/strerr.h>

#define BEVT_DEBUG_LOG_INFO(...) bevt_debug_log(BEVT_DEBUG_INFO, __PRETTY_FUNCTION__, __VA_ARGS__)
#define BEVT_DEBUG_LOG_WARNING(...) bevt_debug_log(BEVT_DEBUG_WARNING, __PRETTY_FUNCTION__, __VA_ARGS__)
#define BEVT_DEBUG_LOG_ERROR(...) bevt_debug_log(BEVT_DEBUG_ERROR, __PRETTY_FUNCTION__, __VA_ARGS__)
#define BEVT_DEBUG_LOG_CRITICAL(...) bevt_debug_log(BEVT_DEBUG_CRITICAL, __PRETTY_FUNCTION__, __VA_ARGS__)

typedef enum {
    BEVT_DEBUG_INFO=0,
    BEVT_DEBUG_WARNING,
    BEVT_DEBUG_ERROR,
    BEVT_DEBUG_CRITICAL
} bevt_debug_type_t;

extern int bevt_debug_log(const bevt_debug_type_t type, const char* fname, const char* restrict fmt, ...);

/**
 *\}
 * End of group
 */

#ifdef __cplusplus
}
#endif

#endif   // _BEVT_DEBUG_PRIV_H_ 
