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

int bevt_relay_parse_prot_cmd(unixmessage_t const *m, void *context) {
    int r;
    bevt_client_id_t id;
    bevt_relay_db_elem_t elem;
    (void)context;

    BEVT_DEBUG_LOG_INFO("message received, len(%d)", m->len);
    if(!strncmp(m->s, bevt_relay_commands[BEVT_RELAY_OP_REGISTER_FIRST], BEVT_RELAY_COMMAND_OP_LEN)) {
        uint64_unpack(m->s+BEVT_RELAY_COMMAND_OP_LEN, &id);
        BEVT_DEBUG_LOG_INFO("register command, id(%llu)", (long long unsigned int)id);
        r = bevt_relay_db_check_reg(id);
        if(!r) {
            BEVT_DEBUG_LOG_INFO("register todo");
            memset(&elem, 0, sizeof(bevt_relay_db_elem_t));
            elem.id = id;
            elem.reg = 1;
            bevt_relay_db_set_elem(&elem);
            answer(0);
        }
        else {
            BEVT_DEBUG_LOG_INFO("register already done");
            answer(EALREADY);
        }

    }
    else if(!strncmp(m->s, bevt_relay_commands[BEVT_RELAY_OP_SUBSCRIBE_FIRST], BEVT_RELAY_COMMAND_OP_LEN)) {
        uint64_unpack(m->s+BEVT_RELAY_COMMAND_OP_LEN, &id);
        BEVT_DEBUG_LOG_INFO("subscribe command, id(%llu)", (long long unsigned int)id);
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

    }
    else if(!strncmp(m->s, bevt_relay_commands[BEVT_RELAY_OP_UNREGISTER], BEVT_RELAY_COMMAND_OP_LEN)) {
        uint64_unpack(m->s+BEVT_RELAY_COMMAND_OP_LEN, &id);
        BEVT_DEBUG_LOG_INFO("unregister command, id(%llu)", (long long unsigned int)id);
        r = bevt_relay_db_get_elem(id, &elem);
        if(!r && elem.reg) {
            BEVT_DEBUG_LOG_INFO("unregister todo");
            elem.reg = 0;
            bevt_relay_db_set_elem(&elem);
            answer(0);
        }
        else {
            BEVT_DEBUG_LOG_INFO("unregister failed");
            answer(ENOENT);
        }

    }
    else if(!strncmp(m->s, bevt_relay_commands[BEVT_RELAY_OP_UNSUBSCRIBE], BEVT_RELAY_COMMAND_OP_LEN)) {
        uint64_unpack(m->s+BEVT_RELAY_COMMAND_OP_LEN, &id);
        BEVT_DEBUG_LOG_INFO("unsubscribe command, id(%llu)", (long long unsigned int)id);
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

    }
    else {
        BEVT_DEBUG_LOG_INFO("unknown command");
        answer(ECANCELED);
    }

    if(0) {
        bozmessage_t c = { .s = m->s, .len = m->len };
        if (!bozmessage_put(&central_sender, &c)) {
            BEVT_DEBUG_LOG_ERROR("unable to put to central");
        }
    }
    return 1;
}
