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
 * \file        bevt_relay_db_set_elem.c
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

static int bevt_relay_db_set_elem_new(bevt_relay_db_elem_t const * const elem) {
    const char* sql_tmpl = "INSERT INTO credentials (id,register,subscribe,notify,reg_prio,sub_prio) VALUES (%llu,%d,%d,%llu,%d,%d);";
    int r;
    int len = UINT64_FMT+1+1+UINT64_FMT+1+strlen(sql_tmpl);
    char sql_req[len];
    snprintf(sql_req, len, sql_tmpl, 
        (long long unsigned int)elem->id, 
        (elem->reg ? 1 : 0), 
        (elem->sub ? 1 : 0), 
        (long long unsigned int)elem->nt, 
        elem->rprio,
        elem->sprio 
        );    
    r = sqlite3_exec(bevt_relayd_db_g, sql_req, 0, 0, 0);
    if(r!=SQLITE_OK)
        return -1;

    return (errno=0,0);
}
    
static int bevt_relay_db_set_elem_upd_reg(const bevt_client_id_t id, const unsigned char reg) {
    const char* sql_tmpl = "UPDATE credentials SET register = %d WHERE id == %llu;";
    int r;
    int len = 1+UINT64_FMT+strlen(sql_tmpl);
    char sql_req[len];
    snprintf(sql_req, len, sql_tmpl, 
        (reg ? 1 : 0), 
        (long long unsigned int)id 
        );    
    r = sqlite3_exec(bevt_relayd_db_g, sql_req, 0, 0, 0);
    if(r!=SQLITE_OK)
        return -1;

    return (errno=0,0);
}
    
static int bevt_relay_db_set_elem_upd_sub(const bevt_client_id_t id, const unsigned char sub) {
    const char* sql_tmpl = "UPDATE credentials SET subscribe = %d WHERE id == %llu;";
    int r;
    int len = 1+UINT64_FMT+strlen(sql_tmpl);
    char sql_req[len];
    snprintf(sql_req, len, sql_tmpl, 
        (sub ? 1 : 0), 
        (long long unsigned int)id 
        );    
    r = sqlite3_exec(bevt_relayd_db_g, sql_req, 0, 0, 0);
    if(r!=SQLITE_OK)
        return -1;

    return (errno=0,0);
}
    
static int bevt_relay_db_set_elem_upd_rprio(const bevt_client_id_t id, const unsigned char prio) {
    const char* sql_tmpl = "UPDATE credentials SET reg_prio = %d WHERE id == %llu;";
    int r;
    int len = 1+UINT64_FMT+strlen(sql_tmpl);
    char sql_req[len];
    snprintf(sql_req, len, sql_tmpl, 
        prio, 
        (long long unsigned int)id 
        );    
    r = sqlite3_exec(bevt_relayd_db_g, sql_req, 0, 0, 0);
    if(r!=SQLITE_OK)
        return -1;

    return (errno=0,0);
}
    
static int bevt_relay_db_set_elem_upd_sprio(const bevt_client_id_t id, const unsigned char prio) {
    const char* sql_tmpl = "UPDATE credentials SET sub_prio = %d WHERE id == %llu;";
    int r;
    int len = 1+UINT64_FMT+strlen(sql_tmpl);
    char sql_req[len];
    snprintf(sql_req, len, sql_tmpl, 
        prio, 
        (long long unsigned int)id 
        );    
    r = sqlite3_exec(bevt_relayd_db_g, sql_req, 0, 0, 0);
    if(r!=SQLITE_OK)
        return -1;

    return (errno=0,0);
}
    
static int bevt_relay_db_set_elem_upd_nt(const bevt_client_id_t id, const unsigned char nt) {
    const char* sql_tmpl = "UPDATE credentials SET notify = %llu WHERE id == %llu;";
    int r;
    int len = UINT64_FMT+UINT64_FMT+strlen(sql_tmpl);
    char sql_req[len];
    snprintf(sql_req, len, sql_tmpl, 
        (long long unsigned int)nt, 
        (long long unsigned int)id 
        );    
    r = sqlite3_exec(bevt_relayd_db_g, sql_req, 0, 0, 0);
    if(r!=SQLITE_OK)
        return -1;

    return (errno=0,0);
}

int bevt_relay_db_set_elem(bevt_relay_db_elem_t const * const elem) {
    int r;
    bevt_relay_db_elem_t telem;

    if(!elem->id) return (errno=EINVAL, -1);

    while (1) {
        r = bevt_relay_db_get_elem(elem->id, &telem);
        if(r<0) {
            /* cretae elem */
            telem.id = elem->id;
            r = bevt_relay_db_set_elem_new(&telem);
            if(r<0) break;
            continue;
        }
        else {
            if(telem.reg != elem->reg) return bevt_relay_db_set_elem_upd_reg(elem->id, elem->reg);
            if(telem.sub != elem->sub) return bevt_relay_db_set_elem_upd_sub(elem->id, elem->sub);
            if(telem.nt < elem->nt) return bevt_relay_db_set_elem_upd_nt(elem->id, elem->nt);
            if(telem.rprio != elem->rprio) return bevt_relay_db_set_elem_upd_rprio(elem->id, elem->rprio);
            if(telem.sprio != elem->sprio) return bevt_relay_db_set_elem_upd_sprio(elem->id, elem->sprio);
            break;
        }
    }

    return (errno=0,r);
}
