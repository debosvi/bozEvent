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
 * \file        bevt_debug_p.c
 * \brief       Message Management private APIs.
 * \version     0.1
 * \date        2013/01/14
 * \author      Vincent de RIBOU.
 * \copyright   Aquaplouf Land.
 *
 */

#include <stdarg.h>
#include <stdio.h>

#include "bevt_debug_p.h"

#define BEVT_DEBUG_BUFFER_SIZE  (256)
static char bevt_debug_buffer[BEVT_DEBUG_BUFFER_SIZE];

//*****************************************************************************
//*****************************************************************************
int bevt_debug_log(const bevt_debug_type_t type, const char* fname, const char* restrict fmt, ...) {
    va_list ap;
    
    va_start(ap, fmt);  
    vsnprintf(&bevt_debug_buffer[0], BEVT_DEBUG_BUFFER_SIZE, fmt, ap);
    va_end(ap);

    switch(type) {

        case BEVT_DEBUG_INFO: 
            strerr_warn3x(fname, ": INFO: ", &bevt_debug_buffer[0]);
            break;
        case BEVT_DEBUG_WARNING: 
            strerr_warn3x(fname, ": WARNING: ", &bevt_debug_buffer[0]); 
            break;
        case BEVT_DEBUG_ERROR: 
            strerr_warn3x(fname, ": ERROR: ", &bevt_debug_buffer[0]);
            break;
        case BEVT_DEBUG_CRITICAL: 
            strerr_warn3x(fname, ": CRITICAL: ", &bevt_debug_buffer[0]); 
            break;
        default: break;
    }

    return 0;
}


