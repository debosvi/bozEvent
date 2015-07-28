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
 * \file        bevt_central_main_socket.c
 * \brief       BozEvent relay client executable.
 * \version     0.1.0
 * \date        2013/01/14
 * \author      Vincent de RIBOU.
 * \copyright   Aquaplouf Land.
 *
 * \brief Implements client relay to central node.
 *
 */

#include <errno.h>
#include <sys/socket.h>

#include "bevt_central_p.h"
#include "bevt_relay_p.h"

//*****************************************************************************
//*****************************************************************************
static int manage_register(const bevt_client_id_t id, boztree_t *t) {
    unsigned int i=0;
    bevt_central_storage_t* data = 0;
    bevt_central_storage_t* tdata = 0;

    for(;i<gensetb_n(&bevt_relay_conns); i++) {
        BEVT_DEBUG_LOG_INFO("%s: loop tree id(%d)", __PRETTY_FUNCTION__, i);
        bevt_central_conn_t *c = gensetb_p(bevt_central_conn_t, &bevt_relay_conns, i);
        boztree_t *taux = &c->t;
        BEVT_DEBUG_LOG_INFO("%s: loop tree (%p)", __PRETTY_FUNCTION__, taux);
        data = boztree_data(taux, id);

        if(!data) continue;

        BEVT_DEBUG_LOG_INFO("%s: rs(%02x)", __PRETTY_FUNCTION__, data->rs);
        if(data->rs & BEVT_CENTRAL_STORAGE_REG_FLAG) {
            return EALREADY;
        }

        if(taux==t)
            tdata=data;

    }

    if(!tdata) {
        BEVT_DEBUG_LOG_INFO("%s: create new leaf (%lld)", __PRETTY_FUNCTION__, (long long int)id);
        bevt_central_storage_t e = BEVT_CENTRAL_STORAGE_ID_INIT(id);
        e.rs |= BEVT_CENTRAL_STORAGE_REG_FLAG;
        boztree_insert(t, (boztree_id_t*)&e);            
    }
    else {
        BEVT_DEBUG_LOG_INFO("%s: update leaf (%lld)", __PRETTY_FUNCTION__, (long long int)id);
        tdata->rs |= BEVT_CENTRAL_STORAGE_REG_FLAG; 
    }

    return 0;
}

//*****************************************************************************
//*****************************************************************************
static int manage_unregister(const bevt_client_id_t id, boztree_t *t) {
    unsigned int i=0;
    bevt_central_storage_t* data = 0;
    bevt_central_storage_t* tdata = 0;

    for(;i<gensetb_n(&bevt_relay_conns); i++) {
        BEVT_DEBUG_LOG_INFO("%s: loop tree id(%d)", __PRETTY_FUNCTION__, i);
        bevt_central_conn_t *c = gensetb_p(bevt_central_conn_t, &bevt_relay_conns, i);
        boztree_t *taux = &c->t;
        BEVT_DEBUG_LOG_INFO("%s: loop tree (%p)", __PRETTY_FUNCTION__, taux);
        data = boztree_data(taux, id);

        if(!data) continue;

        BEVT_DEBUG_LOG_INFO("%s: rs(%02x)", __PRETTY_FUNCTION__, data->rs);
        if(!(data->rs & BEVT_CENTRAL_STORAGE_REG_FLAG)) {
            return EALREADY;
        }

        if(taux==t)
            tdata=data;

    }


    if(!tdata) {
        return ENOENT;
    }
    else {
        BEVT_DEBUG_LOG_INFO("%s: update leaf (%lld)", __PRETTY_FUNCTION__, (long long int)id);
        tdata->rs ^= BEVT_CENTRAL_STORAGE_REG_FLAG; 
    }

    return 0;
}

//*****************************************************************************
//*****************************************************************************
static int manage_subscribe(const bevt_client_id_t id, boztree_t *t) {
    unsigned int i=0;
    bevt_central_storage_t* data = 0;
    bevt_central_storage_t* tdata = 0;

    for(;i<gensetb_n(&bevt_relay_conns); i++) {
        BEVT_DEBUG_LOG_INFO("%s: loop tree id(%d)", __PRETTY_FUNCTION__, i);
        bevt_central_conn_t *c = gensetb_p(bevt_central_conn_t, &bevt_relay_conns, i);
        boztree_t *taux = &c->t;
        BEVT_DEBUG_LOG_INFO("%s: loop tree (%p)", __PRETTY_FUNCTION__, taux);
        data = boztree_data(taux, id);

        if(!data) continue;

        BEVT_DEBUG_LOG_INFO("%s: rs(%02x)", __PRETTY_FUNCTION__, data->rs);
        if(data->rs & BEVT_CENTRAL_STORAGE_SUB_FLAG) {
            return EALREADY;
        }

        if(taux==t)
            tdata=data;

    }


    if(!tdata) {
        BEVT_DEBUG_LOG_INFO("%s: create new leaf (%lld)", __PRETTY_FUNCTION__, (long long int)id);
        bevt_central_storage_t e = BEVT_CENTRAL_STORAGE_ID_INIT(id);
        e.rs |= BEVT_CENTRAL_STORAGE_SUB_FLAG;
        boztree_insert(t, (boztree_id_t*)&e);            
    }
    else {
        BEVT_DEBUG_LOG_INFO("%s: update leaf (%lld)", __PRETTY_FUNCTION__, (long long int)id);
        tdata->rs |= BEVT_CENTRAL_STORAGE_SUB_FLAG; 
    }

    return 0;
}

//*****************************************************************************
//*****************************************************************************
static int manage_unsubscribe(const bevt_client_id_t id, boztree_t *t) {
    unsigned int i=0;
    bevt_central_storage_t* data = 0;
    bevt_central_storage_t* tdata = 0;

    for(;i<gensetb_n(&bevt_relay_conns); i++) {
        BEVT_DEBUG_LOG_INFO("%s: loop tree id(%d)", __PRETTY_FUNCTION__, i);
        bevt_central_conn_t *c = gensetb_p(bevt_central_conn_t, &bevt_relay_conns, i);
        boztree_t *taux = &c->t;
        BEVT_DEBUG_LOG_INFO("%s: loop tree (%p)", __PRETTY_FUNCTION__, taux);
        data = boztree_data(taux, id);

        if(!data) continue;

        BEVT_DEBUG_LOG_INFO("%s: rs(%02x)", __PRETTY_FUNCTION__, data->rs);
        if(!(data->rs & BEVT_CENTRAL_STORAGE_SUB_FLAG)) {
            return EALREADY;
        }

        if(taux==t)
            tdata=data;

    }


    if(!tdata) {
        return ENOENT;
    }
    else {
        BEVT_DEBUG_LOG_INFO("%s: update leaf (%lld)", __PRETTY_FUNCTION__, (long long int)id);
        tdata->rs ^= BEVT_CENTRAL_STORAGE_SUB_FLAG; 
    }

    return 0;
}

//*****************************************************************************
//*****************************************************************************
static int manage_notify(const bevt_client_id_t id, boztree_t *t, const char const *buf, const unsigned int len) {
    bevt_central_storage_t* data = boztree_data(t, id);
    (void)buf;
    (void)len;

    if(!data) return ENOENT;

    BEVT_DEBUG_LOG_INFO("%s: notify id(%lld), len(%u)", __PRETTY_FUNCTION__, (long long int)id, len);
    return 0;
}

//*****************************************************************************
//*****************************************************************************
int bevt_central_parse_prot_cmd(bozmessage_t const *m, void *p) {
    char ret = 0;
    unsigned int len=0;
    bevt_client_id_t id;
    bevt_central_conn_t* c = (bevt_central_conn_t*)p;

    BEVT_DEBUG_LOG_INFO("message received, len(%d)", m->len);

    if(!strncmp(m->s, bevt_relay_commands[BEVT_RELAY_OP_REGISTER_FIRST], BEVT_RELAY_COMMAND_OP_LEN)) {
        uint64_unpack_big(m->s+BEVT_RELAY_COMMAND_OP_LEN, &id);
        BEVT_DEBUG_LOG_INFO("register command, id(%llu)", (long long unsigned int)id);
        ret = manage_register(id, &c->t);
    }
    else if(!strncmp(m->s, bevt_relay_commands[BEVT_RELAY_OP_UNREGISTER], BEVT_RELAY_COMMAND_OP_LEN)) {
        uint64_unpack_big(m->s+BEVT_RELAY_COMMAND_OP_LEN, &id);
        BEVT_DEBUG_LOG_INFO("unregister command, id(%llu)", (long long unsigned int)id);
        ret = manage_unregister(id, &c->t);
    }
    else if(!strncmp(m->s, bevt_relay_commands[BEVT_RELAY_OP_SUBSCRIBE_FIRST], BEVT_RELAY_COMMAND_OP_LEN)) {
        uint64_unpack_big(m->s+BEVT_RELAY_COMMAND_OP_LEN, &id);
        BEVT_DEBUG_LOG_INFO("subscribe command, id(%llu)", (long long unsigned int)id);
        ret = manage_subscribe(id, &c->t);
    }
    else if(!strncmp(m->s, bevt_relay_commands[BEVT_RELAY_OP_UNSUBSCRIBE], BEVT_RELAY_COMMAND_OP_LEN)) {
        uint64_unpack_big(m->s+BEVT_RELAY_COMMAND_OP_LEN, &id);
        BEVT_DEBUG_LOG_INFO("unsubscribe command, id(%llu)", (long long unsigned int)id);
        ret = manage_unsubscribe(id, &c->t);
    }
    else if(!strncmp(m->s, bevt_relay_commands[BEVT_RELAY_OP_NOTIFY], BEVT_RELAY_COMMAND_OP_LEN)) {
        uint64_unpack_big(m->s+BEVT_RELAY_COMMAND_OP_LEN, &id);
        uint32_unpack_big(m->s+BEVT_RELAY_COMMAND_OP_LEN+8, &len);
        BEVT_DEBUG_LOG_INFO("unsubscribe command, id(%llu)", (long long unsigned int)id);
        ret = manage_notify(id, &c->t, m->s+BEVT_RELAY_COMMAND_OP_LEN+8+4, len);
    }
    else {
        BEVT_DEBUG_LOG_INFO("unknown command");
        ret = EBADMSG;
    }

    {
        bozmessage_t e = { .s = &ret, .len = 1 };
        bozmessage_put(&c->out, &e);
        bozmessage_sender_flush(&c->out);
    }
    return 1;
}
    
