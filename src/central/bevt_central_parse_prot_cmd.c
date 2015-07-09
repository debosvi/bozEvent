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
    int ret = 0;
    bevt_central_storage_t* data = boztree_data(t, id);
    if(!data) {
        bevt_central_storage_t e = BEVT_CENTRAL_STORAGE_ID_INIT(id);
        e.rs |= BEVT_CENTRAL_STORAGE_REG_FLAG;
        boztree_insert(t, (boztree_id_t*)&e);            
    }
    else {
        if(data->rs & BEVT_CENTRAL_STORAGE_REG_FLAG)
            ret = EALREADY;
        else data->rs |= BEVT_CENTRAL_STORAGE_REG_FLAG; 
    
        BEVT_DEBUG_LOG_INFO("%s: rs(%02x)", __PRETTY_FUNCTION__, data->rs);
    }

    return ret;
}

//*****************************************************************************
//*****************************************************************************
static int manage_unregister(const bevt_client_id_t id, boztree_t *t) {
    int ret = 0;
    bevt_central_storage_t* data = boztree_data(t, id);
    if(!data) {
        bevt_central_storage_t e = BEVT_CENTRAL_STORAGE_ID_INIT(id);
        boztree_insert(t, (boztree_id_t*)&e);            
    }
    else {
        if(!(data->rs & BEVT_CENTRAL_STORAGE_REG_FLAG))
            ret = EALREADY;
        else data->rs ^= BEVT_CENTRAL_STORAGE_REG_FLAG;

        BEVT_DEBUG_LOG_INFO("%s: rs(%02x)", __PRETTY_FUNCTION__, data->rs);
    }

    return ret;
}

//*****************************************************************************
//*****************************************************************************
static int manage_subscribe(const bevt_client_id_t id, boztree_t *t) {
    int ret = 0;
    bevt_central_storage_t* data = boztree_data(t, id);
    if(!data) {
        bevt_central_storage_t e = BEVT_CENTRAL_STORAGE_ID_INIT(id);
        e.rs |= BEVT_CENTRAL_STORAGE_SUB_FLAG;
        boztree_insert(t, (boztree_id_t*)&e);            
    }
    else {
        if(data->rs & BEVT_CENTRAL_STORAGE_SUB_FLAG)
            ret = EALREADY;
        else data->rs |= BEVT_CENTRAL_STORAGE_SUB_FLAG; 

        BEVT_DEBUG_LOG_INFO("%s: rs(%02x)", __PRETTY_FUNCTION__, data->rs);
    }

    return ret;
}

//*****************************************************************************
//*****************************************************************************
static int manage_unsubscribe(const bevt_client_id_t id, boztree_t *t) {
    int ret = 0;
    bevt_central_storage_t* data = boztree_data(t, id);
    if(!data) {
        bevt_central_storage_t e = BEVT_CENTRAL_STORAGE_ID_INIT(id);
        boztree_insert(t, (boztree_id_t*)&e);            
    }
    else {
        if(!(data->rs & BEVT_CENTRAL_STORAGE_SUB_FLAG))
            ret = EALREADY;
        else data->rs ^= BEVT_CENTRAL_STORAGE_SUB_FLAG;
    
        BEVT_DEBUG_LOG_INFO("%s: rs(%02x)", __PRETTY_FUNCTION__, data->rs);
    }

    return ret;
}

//*****************************************************************************
//*****************************************************************************
int bevt_central_parse_prot_cmd(bozmessage_t const *m, void *p) {
    char ret = 0;
    bevt_client_id_t id;
    bevt_central_conn_t* c = (bevt_central_conn_t*)p;

    BEVT_DEBUG_LOG_INFO("message received, len(%d)", m->len);

    if(!strncmp(m->s, bevt_relay_commands[BEVT_RELAY_OP_REGISTER_FIRST], BEVT_RELAY_COMMAND_OP_LEN)) {
        uint64_unpack(m->s+BEVT_RELAY_COMMAND_OP_LEN, &id);
        BEVT_DEBUG_LOG_INFO("register command, id(%llu)", (long long unsigned int)id);
        ret = manage_register(id, &c->t);
    }
    else if(!strncmp(m->s, bevt_relay_commands[BEVT_RELAY_OP_UNREGISTER], BEVT_RELAY_COMMAND_OP_LEN)) {
        uint64_unpack(m->s+BEVT_RELAY_COMMAND_OP_LEN, &id);
        BEVT_DEBUG_LOG_INFO("unregister command, id(%llu)", (long long unsigned int)id);
        ret = manage_unregister(id, &c->t);
    }
    else if(!strncmp(m->s, bevt_relay_commands[BEVT_RELAY_OP_SUBSCRIBE_FIRST], BEVT_RELAY_COMMAND_OP_LEN)) {
        uint64_unpack(m->s+BEVT_RELAY_COMMAND_OP_LEN, &id);
        BEVT_DEBUG_LOG_INFO("subscribe command, id(%llu)", (long long unsigned int)id);
        ret = manage_subscribe(id, &c->t);
    }
    else if(!strncmp(m->s, bevt_relay_commands[BEVT_RELAY_OP_UNSUBSCRIBE], BEVT_RELAY_COMMAND_OP_LEN)) {
        uint64_unpack(m->s+BEVT_RELAY_COMMAND_OP_LEN, &id);
        BEVT_DEBUG_LOG_INFO("unsubscribe command, id(%llu)", (long long unsigned int)id);
        ret = manage_unsubscribe(id, &c->t);
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
    
