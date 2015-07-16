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
 * \file        bevt_relay_db_init.c
 * \brief       BozEvent relay client executable.
 * \version     0.1.0
 * \date        2013/01/14
 * \author      Vincent de RIBOU.
 * \copyright   Aquaplouf Land.
 *
 * \brief Implements client relay to central node.
 *
 */

#include <unistd.h>
#include <errno.h>
#include <stdio.h>

#include <skalibs/environ.h>
#include <skalibs/env.h>

#include "bevt_relay_p.h"

int bevt_relay_db_init(char const *db_name) {
    int ret=0;
    char const * restart;

    restart = env_get("BEVT_RELAY_RESTART");
    stralloc_cats(&bevt_relay_db_name_g, "/tmp/boz/bevt/");
    stralloc_cats(&bevt_relay_db_name_g, db_name);

    if(!restart) {
        BEVT_DEBUG_LOG_INFO("db init on (%s)", db_name);
        rm_rf(bevt_relay_db_name_g.s);
    }
    else {
        BEVT_DEBUG_LOG_INFO("reuse db (%s)", db_name);
    }

    if(!restart) {
        stralloc first = STRALLOC_ZERO;
        stralloc_cats(&first, "#id:reg:sub:prio:notify\n");
        if(!openwritenclose(bevt_relay_db_name_g.s, first.s, first.len)) {
            BEVT_DEBUG_LOG_ERROR("db init on (%s) failed, errno(%d/%s)", db_name, errno, strerror(errno));
            ret=-1;
        }
        stralloc_free(&first);
    }

    if(!ret) errno=0;
    return ret;
}
