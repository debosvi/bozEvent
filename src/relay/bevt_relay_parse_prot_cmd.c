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
 * \file        bevt_relay_parse_prot_cmd.c
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

#include "bevt_relay_p.h"

//*****************************************************************************
//*****************************************************************************
static void answer(char c) {
    unixmessage_t am = { .s = &c, .len = 1, .fds = 0, .nfds = 0 } ;
    if (!unixmessage_put(unixmessage_sender_1, &am)) {
        BEVT_DEBUG_LOG_ERROR("unable to put ack");
    }
    else if (!unixmessage_sender_flush(unixmessage_sender_1)) {
        BEVT_DEBUG_LOG_ERROR("unable to send ack");
    }
    else {
        BEVT_DEBUG_LOG_INFO("ack sent");
    }
}

//*****************************************************************************
//*****************************************************************************
static int send_to_central(const bevt_client_id_t id, const bevt_relay_op_t op) {
    int err;
    char fmt_id[8];
    siovec_t v[2] = { 
        { .s = (char*)bevt_relay_commands[op], .len = BEVT_RELAY_COMMAND_OP_LEN }, 
        { .s = &fmt_id[0], .len = 8}
    };
    uint64_pack(&fmt_id[0], id);

    if (!bozclient_putv(&central_client_g, v, 2, bozclient_default_cb, &err)) {
        BEVT_DEBUG_LOG_ERROR("unable to put to central");
    }
    else {
        tain_t deadline;
        tain_now_g();
        tain_addsec_g(&deadline, 1);

        if(!bozclient_syncify(&central_client_g, &deadline, &STAMP)) {
            BEVT_DEBUG_LOG_ERROR("unable to send to central");
        }   
        else {
            BEVT_DEBUG_LOG_ERROR("central acks with (%d/%s)", err, strerror(err));
            if(err) return (errno=err,-1);
        }
        
    }

    return (errno=0,0);
}

//*****************************************************************************
//*****************************************************************************
static int manage_register(const bevt_client_id_t id) {
    int r;
    bevt_relay_db_elem_t elem;

    r = bevt_relay_db_check_reg(id);
    if(!r) {
        BEVT_DEBUG_LOG_INFO("register todo");
        r = send_to_central(id, BEVT_RELAY_OP_REGISTER_FIRST);
        if(!r) {
            memset(&elem, 0, sizeof(bevt_relay_db_elem_t));
            elem.id = id;
            elem.reg = 1;
            bevt_relay_db_set_elem(&elem);
            answer(0);
        }
        else {
            answer(errno);
        }
    }
    else {
        BEVT_DEBUG_LOG_INFO("register already done");
        answer(EALREADY);
    }
    return 0; 
}

//*****************************************************************************
//*****************************************************************************
static int manage_subscribe(const bevt_client_id_t id) {
    int r;
    bevt_relay_db_elem_t elem;

    r = bevt_relay_db_check_reg(id);
    if(!r) {
        BEVT_DEBUG_LOG_INFO("subscribe todo");
        memset(&elem, 0, sizeof(bevt_relay_db_elem_t));
        elem.id = id;
        elem.sub = 1;
        bevt_relay_db_set_elem(&elem);
        answer(0);
    }
    else {
        BEVT_DEBUG_LOG_INFO("subscribe already done");
        answer(EALREADY);
    }

    return 0; 
}

//*****************************************************************************
//*****************************************************************************
static int manage_unregister(const bevt_client_id_t id) {
    int r;
    bevt_relay_db_elem_t elem;

    r = bevt_relay_db_get_elem(id, &elem);
    if(!r && elem.reg) {
        BEVT_DEBUG_LOG_INFO("unregister todo");
        r = send_to_central(id, BEVT_RELAY_OP_UNREGISTER);
        if(!r) {
            memset(&elem, 0, sizeof(bevt_relay_db_elem_t));
            elem.id = id;
            elem.reg = 0;
            bevt_relay_db_set_elem(&elem);
            answer(0);
        }
        else {
            answer(errno);
        }
    }
    else {
        BEVT_DEBUG_LOG_INFO("unregister failed");
        answer(ENOENT);
    }

    return 0; 
}

//*****************************************************************************
//*****************************************************************************
static int manage_unsubscribe(const bevt_client_id_t id) {
    int r;
    bevt_relay_db_elem_t elem;

    r = bevt_relay_db_get_elem(id, &elem);
    if(!r && elem.sub) {
        BEVT_DEBUG_LOG_INFO("unsubscribe todo");
        elem.sub = 0;
        bevt_relay_db_set_elem(&elem);
        answer(0);
    }
    else {
        BEVT_DEBUG_LOG_INFO("unsubscribe failed");
        answer(ENOENT);
    }

    return 0; 
}

//*****************************************************************************
//*****************************************************************************
int bevt_relay_parse_prot_cmd(unixmessage_t const *m, void *context) {
    int r;
    bevt_client_id_t id;
    (void)context;

    BEVT_DEBUG_LOG_INFO("message received, len(%d)", m->len);
    if(!strncmp(m->s, bevt_relay_commands[BEVT_RELAY_OP_REGISTER_FIRST], BEVT_RELAY_COMMAND_OP_LEN)) {
        uint64_unpack(m->s+BEVT_RELAY_COMMAND_OP_LEN, &id);
        BEVT_DEBUG_LOG_INFO("register command, id(%llu)", (long long unsigned int)id);
        r = manage_register(id);
    }
    else if(!strncmp(m->s, bevt_relay_commands[BEVT_RELAY_OP_SUBSCRIBE_FIRST], BEVT_RELAY_COMMAND_OP_LEN)) {
        uint64_unpack(m->s+BEVT_RELAY_COMMAND_OP_LEN, &id);
        BEVT_DEBUG_LOG_INFO("subscribe command, id(%llu)", (long long unsigned int)id);
        r = manage_subscribe(id);
    }
    else if(!strncmp(m->s, bevt_relay_commands[BEVT_RELAY_OP_UNREGISTER], BEVT_RELAY_COMMAND_OP_LEN)) {
        uint64_unpack(m->s+BEVT_RELAY_COMMAND_OP_LEN, &id);
        BEVT_DEBUG_LOG_INFO("unregister command, id(%llu)", (long long unsigned int)id);
        r = manage_unregister(id);
    }
    else if(!strncmp(m->s, bevt_relay_commands[BEVT_RELAY_OP_UNSUBSCRIBE], BEVT_RELAY_COMMAND_OP_LEN)) {
        uint64_unpack(m->s+BEVT_RELAY_COMMAND_OP_LEN, &id);
        BEVT_DEBUG_LOG_INFO("unsubscribe command, id(%llu)", (long long unsigned int)id);
        r = manage_unsubscribe(id);
    }
    else {
        BEVT_DEBUG_LOG_INFO("unknown command");
        answer(ECANCELED);
    }

    if(!r) errno=0;
    return 1;
}
