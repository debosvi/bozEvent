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
 * \file        bevt_client_register.c
 * \brief       Message creation implementation.
 * \version     0.1
 * \date        2013/01/14
 * \author      Vincent de RIBOU.
 * \copyright   Aquaplouf Land.
 *
 */

#include <errno.h>

#include <skalibs/uint64.h>
#include <skalibs/tai.h>
#include <skalibs/siovec.h>

#include "bevt_client_p.h"

//*****************************************************************************
//*****************************************************************************
int bevt_client_register(const bevt_client_id_t id, const bevt_client_prio_t prio) {
    int r;

    if( !id ||
        (prio<BEVT_CLIENT_PRIO_DEFAULT) ||
        (prio>BEVT_CLIENT_PRIO_MAJOR) 
        )  {
        BEVT_DEBUG_LOG_ERROR("Bad parameters");
        return (errno=EINVAL, -1);
    }

    {
        char err;
        tain_t deadline;
        char fmt_id[8];
        char fmt_prio[2];
        siovec_t v[3] = { 
            { .s = (char*)bevt_relay_commands[BEVT_RELAY_OP_REGISTER_FIRST], .len = BEVT_RELAY_COMMAND_OP_LEN }, 
            { .s = &fmt_id[0], .len = 8 },
            { .s = &fmt_prio[0], .len = 2 } 
        };
        uint64_pack_big(&fmt_id[0], id);
        uint16_pack_big(&fmt_prio[0], prio);
        tain_now_g();
        tain_addsec_g(&deadline, 1);
        r = skaclient_sendv (&bevt_client_g.connection, &v[0], 3, &skaclient_default_cb, &err, &deadline, &STAMP);    
        if(!r || err) {
            BEVT_DEBUG_LOG_ERROR("send command to relay failed");
            if(err) errno=err;
            return -1;
        } 
    }

    return (errno=0,0);
}

