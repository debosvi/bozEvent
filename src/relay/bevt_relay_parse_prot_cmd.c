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
static void sync_answer(char c) {
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
    uint64_pack_big(&fmt_id[0], id);

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
static int notify_to_central(const bevt_client_id_t id, const char const *data, const unsigned int len) {
    int err;
    char fmt_id[8];
    char fmt_lg[4];
    siovec_t v[4] = {
        { .s = (char*)bevt_relay_commands[BEVT_RELAY_OP_NOTIFY], .len = BEVT_RELAY_COMMAND_OP_LEN },
        { .s = &fmt_id[0], .len = 8 },
        { .s = &fmt_lg[0], .len = 4 },
        { .s = (char*)data, .len = len }
    };
    uint64_pack_big(&fmt_id[0], id);
    uint32_pack_big(&fmt_lg[0], len);

    if (!bozclient_putv(&central_client_g, v, 4, bozclient_default_cb, &err)) {
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
static int manage_notify(const bevt_client_id_t id, const char const *data, const unsigned int len) {
    int r;

    BEVT_DEBUG_LOG_INFO("id(%llu), len(%u)", (long long int)id, len);
    r = bevt_relay_db_check_reg(id);
    if(r) {
        BEVT_DEBUG_LOG_INFO("notify todo");
        r = notify_to_central(id, data, len);
        sync_answer(errno);
    }
    else {
        BEVT_DEBUG_LOG_INFO("notify impossible since no register done");
        sync_answer(ENOENT);
    }

    return 0;
}

//*****************************************************************************
//*****************************************************************************
static int manage_register(const bevt_client_id_t id, const bevt_client_prio_t prio) {
    int r;
    bevt_relay_db_elem_t elem;

    BEVT_DEBUG_LOG_INFO("id(%llu), prio(%u)", (long long int)id, prio);
    r = bevt_relay_db_check_reg(id);
    if(!r) {
        BEVT_DEBUG_LOG_INFO("register todo");
        r = send_to_central(id, BEVT_RELAY_OP_REGISTER_FIRST);
        if(!r) {
            if(bevt_relay_db_get_elem(id, &elem))
                memset(&elem, 0, sizeof(bevt_relay_db_elem_t));
            elem.id = id;
            elem.reg = 1;
            elem.rprio = prio;
            bevt_relay_db_set_elem(&elem);
            sync_answer(0);
        }
        else {
            sync_answer(errno);
        }
    }
    else {
        BEVT_DEBUG_LOG_INFO("register already done");
        sync_answer(EALREADY);
    }
    return 0; 
}

//*****************************************************************************
//*****************************************************************************
static int manage_register_update(const bevt_client_id_t id, const bevt_client_prio_t prio) {
    int r;
    bevt_relay_db_elem_t elem;

    BEVT_DEBUG_LOG_INFO("id(%llu), prio(%u)", (long long int)id, prio);
    r = bevt_relay_db_get_elem(id, &elem);
    if(!r && elem.reg && (elem.rprio!=prio)) {
        BEVT_DEBUG_LOG_INFO("register update todo");
        elem.rprio = prio;
        bevt_relay_db_set_elem(&elem);
        sync_answer(0);
    }
    else {
        BEVT_DEBUG_LOG_INFO("register update impossible/not relevant");
        sync_answer(elem.rprio==prio ? EALREADY : ENOENT);
    }
    return 0; 
}

//*****************************************************************************
//*****************************************************************************
static int manage_subscribe(const bevt_client_id_t id, const bevt_client_prio_t prio) {
    int r;
    bevt_relay_db_elem_t elem;

    BEVT_DEBUG_LOG_INFO("id(%llu), prio(%u)", (long long int)id, prio);
    r = bevt_relay_db_check_sub(id);
    if(!r) {
        BEVT_DEBUG_LOG_INFO("subscribe todo");
        r = send_to_central(id, BEVT_RELAY_OP_SUBSCRIBE_FIRST);
        if(!r) {
            if(bevt_relay_db_get_elem(id, &elem))
                memset(&elem, 0, sizeof(bevt_relay_db_elem_t));
            elem.id = id;
            elem.sub = 1;
            elem.sprio = prio;
            bevt_relay_db_set_elem(&elem);
            sync_answer(0);
        }
        else {
            sync_answer(errno);
        }
    }
    else {
        BEVT_DEBUG_LOG_INFO("subscribe already done");
        sync_answer(EALREADY);
    }

    return 0; 
}

//*****************************************************************************
//*****************************************************************************
static int manage_subscribe_update(const bevt_client_id_t id, const bevt_client_prio_t prio) {
    int r;
    bevt_relay_db_elem_t elem;

    BEVT_DEBUG_LOG_INFO("id(%llu), prio(%u)", (long long int)id, prio);
    r = bevt_relay_db_get_elem(id, &elem);
    if(!r && elem.sub && (elem.sprio!=prio)) {
        BEVT_DEBUG_LOG_INFO("subscribe update todo");
        elem.sprio = prio;
        bevt_relay_db_set_elem(&elem);
        sync_answer(0);
    }
    else {
        BEVT_DEBUG_LOG_INFO("subscribe update impossible/not relevant");
        sync_answer(elem.sprio==prio ? EALREADY : ENOENT);
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
            elem.id = id;
            elem.reg = 0;
            bevt_relay_db_set_elem(&elem);
            sync_answer(0);
        }
        else {
            sync_answer(errno);
        }
    }
    else {
        BEVT_DEBUG_LOG_INFO("unregister failed");
        sync_answer(ENOENT);
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
        r = send_to_central(id, BEVT_RELAY_OP_UNSUBSCRIBE);
        if(!r) {
            elem.id = id;
            elem.sub = 0;
            bevt_relay_db_set_elem(&elem);
            sync_answer(0);
        }
        else {
            sync_answer(errno);
        }
    }
    else {
        BEVT_DEBUG_LOG_INFO("unsubscribe failed");
        sync_answer(ENOENT);
    }

    return 0; 
}

//*****************************************************************************
//*****************************************************************************
int bevt_relay_parse_prot_cmd(unixmessage_t const *m, void *context) {
    int r;
    bevt_client_id_t id;
    bevt_client_prio_t prio = 0;
    uint32 lg=0;
    (void)context;

    BEVT_DEBUG_LOG_INFO("message received, len(%d)", m->len);
    for(r=0; r<(int)m->len; r++)
        BEVT_DEBUG_LOG_INFO("%02x ", m->s[r]);

    if(!strncmp(m->s, bevt_relay_commands[BEVT_RELAY_OP_REGISTER_FIRST], BEVT_RELAY_COMMAND_OP_LEN)) {
        uint64_unpack_big(m->s+BEVT_RELAY_COMMAND_OP_LEN, &id);
        uint16_unpack_big(m->s+BEVT_RELAY_COMMAND_OP_LEN+8, (uint16*)&prio);
        BEVT_DEBUG_LOG_INFO("register (first) command, id(%llu), prio(%u)", (long long unsigned int)id, prio);
        r = manage_register(id, prio);
    }
    else if(!strncmp(m->s, bevt_relay_commands[BEVT_RELAY_OP_REGISTER_UPDATE], BEVT_RELAY_COMMAND_OP_LEN)) {
        uint64_unpack_big(m->s+BEVT_RELAY_COMMAND_OP_LEN, &id);
        uint16_unpack_big(m->s+BEVT_RELAY_COMMAND_OP_LEN+8, (uint16*)&prio);
        BEVT_DEBUG_LOG_INFO("register (update) command, id(%llu), prio(%u)", (long long unsigned int)id, prio);
        r = manage_register_update(id, prio);
    }
    else if(!strncmp(m->s, bevt_relay_commands[BEVT_RELAY_OP_SUBSCRIBE_FIRST], BEVT_RELAY_COMMAND_OP_LEN)) {
        uint64_unpack_big(m->s+BEVT_RELAY_COMMAND_OP_LEN, &id);
        uint16_unpack_big(m->s+BEVT_RELAY_COMMAND_OP_LEN+8, (uint16*)&prio);
        BEVT_DEBUG_LOG_INFO("subscribe (first) command, id(%llu), prio(%u)", (long long unsigned int)id, prio);
        r = manage_subscribe(id, prio);
    }
    else if(!strncmp(m->s, bevt_relay_commands[BEVT_RELAY_OP_SUBSCRIBE_UPDATE], BEVT_RELAY_COMMAND_OP_LEN)) {
        uint64_unpack_big(m->s+BEVT_RELAY_COMMAND_OP_LEN, &id);
        uint16_unpack_big(m->s+BEVT_RELAY_COMMAND_OP_LEN+8, (uint16*)&prio);
        BEVT_DEBUG_LOG_INFO("subscribe (update) command, id(%llu), prio(%u)", (long long unsigned int)id, prio);
        r = manage_subscribe_update(id, prio);
    }
    else if(!strncmp(m->s, bevt_relay_commands[BEVT_RELAY_OP_UNREGISTER], BEVT_RELAY_COMMAND_OP_LEN)) {
        uint64_unpack_big(m->s+BEVT_RELAY_COMMAND_OP_LEN, &id);
        BEVT_DEBUG_LOG_INFO("unregister command, id(%llu)", (long long unsigned int)id);
        r = manage_unregister(id);
    }
    else if(!strncmp(m->s, bevt_relay_commands[BEVT_RELAY_OP_UNSUBSCRIBE], BEVT_RELAY_COMMAND_OP_LEN)) {
        uint64_unpack_big(m->s+BEVT_RELAY_COMMAND_OP_LEN, &id);
        BEVT_DEBUG_LOG_INFO("unsubscribe command, id(%llu)", (long long unsigned int)id);
        r = manage_unsubscribe(id);
    }
    else if(!strncmp(m->s, bevt_relay_commands[BEVT_RELAY_OP_NOTIFY], BEVT_RELAY_COMMAND_OP_LEN)) {
        uint64_unpack_big(m->s+BEVT_RELAY_COMMAND_OP_LEN, &id);
        uint32_unpack_big(m->s+BEVT_RELAY_COMMAND_OP_LEN+8, &lg);
        BEVT_DEBUG_LOG_INFO("notify command, id(%llu)", (long long unsigned int)id);
        r = manage_notify(id, m->s+BEVT_RELAY_COMMAND_OP_LEN+12, lg);
    }
    else {
        BEVT_DEBUG_LOG_INFO("unknown command");
        sync_answer(ECANCELED);
    }

    if(!r) errno=0;
    return 1;
}
