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

static bevt_client_id_t last_id = 0;

int bevt_central_parse_prot_cmd(bozmessage_t const *m, void *p) {
    char ret = 0;
    bevt_client_id_t id;
    bozmessage_sender_t* s = (bozmessage_sender_t*)p;

    BEVT_DEBUG_LOG_INFO("message received, len(%d)", m->len);

    if(!strncmp(m->s, bevt_relay_commands[BEVT_RELAY_OP_REGISTER_FIRST], BEVT_RELAY_COMMAND_OP_LEN)) {
        uint64_unpack(m->s+BEVT_RELAY_COMMAND_OP_LEN, &id);
        BEVT_DEBUG_LOG_INFO("register command, id(%llu)", (long long unsigned int)id);
        if(id!=last_id) {
            last_id = id;
        }
        else {
            ret=EALREADY;
        }
    }
    else if(!strncmp(m->s, bevt_relay_commands[BEVT_RELAY_OP_UNREGISTER], BEVT_RELAY_COMMAND_OP_LEN)) {
        uint64_unpack(m->s+BEVT_RELAY_COMMAND_OP_LEN, &id);
        BEVT_DEBUG_LOG_INFO("unregister command, id(%llu)", (long long unsigned int)id);
        if(id==last_id) {
            last_id = 0;
        }
        else {
            ret=EALREADY;
        }


    }
    else if(!strncmp(m->s, bevt_relay_commands[BEVT_RELAY_OP_SUBSCRIBE_FIRST], BEVT_RELAY_COMMAND_OP_LEN)) {
        uint64_unpack(m->s+BEVT_RELAY_COMMAND_OP_LEN, &id);
        BEVT_DEBUG_LOG_INFO("subscribe command, id(%llu)", (long long unsigned int)id);


    }
    else if(!strncmp(m->s, bevt_relay_commands[BEVT_RELAY_OP_UNSUBSCRIBE], BEVT_RELAY_COMMAND_OP_LEN)) {
        uint64_unpack(m->s+BEVT_RELAY_COMMAND_OP_LEN, &id);
        BEVT_DEBUG_LOG_INFO("unsubscribe command, id(%llu)", (long long unsigned int)id);


    }
    else {
        BEVT_DEBUG_LOG_INFO("unknown command");
        ret = EBADMSG;
    }

    {
        bozmessage_t e = { .s = &ret, .len = 1 };
        bozmessage_put(s, &e);
        bozmessage_sender_flush(s);
    }
    return 1;
}
    
