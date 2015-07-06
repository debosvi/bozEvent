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
 * \file        bevt_relay_db_get_elem.c
 * \brief       BozEvent relay client executable.
 * \version     0.1.0
 * \date        2013/01/14
 * \author      Vincent de RIBOU.
 * \copyright   Aquaplouf Land.
 *
 * \brief Implements client relay to central node.
 *
 */

#include <stdio.h>

#include "bevt_relay_p.h"

static const char* bevt_relay_db_reg_sql = "SELECT * FROM credentials where id == %d;";

static int bevt_relay_db_elem_cb(void *data, int argc, char **argv, char **azColName) {
    int i;
    bevt_relay_db_elem_t *elem = (bevt_relay_db_elem_t*)data;
    if(!argc) return 1;

    for(i=0; i<argc; i++) {
        if(!strncmp("id", azColName[i], 2))
            uint64_scan(argv[i], &elem->id);
        else if(!strncmp("notify", azColName[i], 6))
            uint64_scan(argv[i], &elem->nt);
        else if(!strncmp("register", azColName[i], 8))
            elem->reg = ((*argv[i]=='0') ? 0 : 1);
        else if(!strncmp("subscribe", azColName[i], 9))
            elem->sub = ((*argv[i]=='0') ? 0 : 1);
        else if(!strncmp("prio", azColName[i], 4))
            elem->prio = *argv[i];
        else
            BEVT_DEBUG_LOG_ERROR("%s: unknown column(%s), value(%s)", __PRETTY_FUNCTION__, azColName[i], argv[i]);
    }
    return 0;
}


int bevt_relay_db_get_elem(const bevt_client_id_t id, bevt_relay_db_elem_t* const elem) {
    int r;
    int len = UINT64_FMT+strlen(bevt_relay_db_reg_sql);
    char sql_req[len];

    memset(elem, 0, sizeof(bevt_relay_db_elem_t));

    snprintf(sql_req, len, bevt_relay_db_reg_sql, id);    

    r = sqlite3_exec(bevt_relayd_db_g, sql_req, bevt_relay_db_elem_cb, elem, 0);
    if(r!=SQLITE_OK)
        return -1;

    if(!elem->id)
        return -1;

    return 0;
}
