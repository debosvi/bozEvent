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
#include <skalibs/djbunix.h>

#include "bevt_relay_p.h"

sqlite3 *bevt_relayd_db_g=0;

int bevt_relay_db_init(char const *db_name) {
    int ret=0, r;
    char const * restart;
    stralloc sa_db = STRALLOC_ZERO;
    stralloc sa_sql = STRALLOC_ZERO;
    stralloc sa_sql2 = STRALLOC_ZERO;

    restart = env_get("BEVT_RELAY_RESTART");
    stralloc_cats(&sa_db, "/tmp/boz/bevt/");
    stralloc_cats(&sa_db, db_name);
    stralloc_cats(&sa_db, ".sqlite");

    if(!restart) {
        BEVT_DEBUG_LOG_INFO("db init on (%s)", db_name);
        rm_rf(sa_db.s);
    }
    else {
        BEVT_DEBUG_LOG_INFO("reuse db (%s)", db_name);
    }

    r = sqlite3_open(sa_db.s, &bevt_relayd_db_g);
    if(r != SQLITE_OK) {
        BEVT_DEBUG_LOG_ERROR("db open failed (%s), msg(%s)", sa_db.s, sqlite3_errmsg(bevt_relayd_db_g));
        ret=-1;
        goto exit;
    }

    if(!restart) {
        stralloc_cats(&sa_sql, SCRIPT_INSTALL_DIR);
        stralloc_cats(&sa_sql, "/bevt_relayd_dbcreate.sql");
        openreadclose(sa_sql.s, &sa_sql2, 0);
        r = sqlite3_exec(bevt_relayd_db_g, sa_sql2.s, 0, 0, 0); 
        if(r != SQLITE_OK) {
            BEVT_DEBUG_LOG_ERROR("db create failed (%s), msg(%s)", sa_db.s, sqlite3_errmsg(bevt_relayd_db_g));
            ret=-1;
            goto exit;
        }
    }

exit:
    stralloc_free(&sa_db);
    stralloc_free(&sa_sql);
    stralloc_free(&sa_sql2);
    if(!ret) errno=0;
    return ret;
}
