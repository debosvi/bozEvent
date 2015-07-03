
#include <stdio.h>

#include "bevt_relay_p.h"

static int bevt_relay_db_set_elem_new(bevt_relay_db_elem_t const * const elem) {
    const char* sql_tmpl = "INSERT INTO credentials (id,register,subscribe,notify,priority) VALUES (%llu,%d,%d,%llu,%d);";
    int r;
    int len = UINT64_FMT+1+1+UINT64_FMT+1+strlen(sql_tmpl);
    char sql_req[len];
    snprintf(sql_req, len, sql_tmpl, 
        (long long unsigned int)elem->id, 
        (elem->reg ? 1 : 0), 
        (elem->sub ? 1 : 0), 
        (long long unsigned int)elem->nt, 
        elem->prio 
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
    
static int bevt_relay_db_set_elem_upd_prio(const bevt_client_id_t id, const unsigned char prio) {
    const char* sql_tmpl = "UPDATE credentials SET priority = %d WHERE id == %llu;";
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
            if(telem.sub != elem->sub) return bevt_relay_db_set_elem_upd_sub(elem->id, elem->reg);
            if(telem.nt < elem->nt) return bevt_relay_db_set_elem_upd_nt(elem->id, elem->reg);
            if(telem.prio != elem->prio) return bevt_relay_db_set_elem_upd_prio(elem->id, elem->reg);
        }
    }

    return (errno=0,r);
}
