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
 * \file        bevt_client_p.c
 * \brief       Message Management private APIs.
 * \version     0.1
 * \date        2013/01/14
 * \author      Vincent de RIBOU.
 * \copyright   Aquaplouf Land.
 *
 */

#include <errno.h>

#include <skalibs/strerr2.h>
#include <skalibs/djbunix.h>
#include <skalibs/selfpipe.h>

#include "bevt_client_p.h"
#include "bevt_relay_p.h"

bevt_client_glob_t bevt_client_g = BEVT_CLIENT_GLOB_ZERO;

//*****************************************************************************
//*****************************************************************************
int bevt_client_start_relay(int b) {
    char const *cargv[3] = { BEVT_RELAY_PROG, &bevt_client_g.rid[0], 0 } ;
    char const *cenvp[2] = { 0, 0 } ;
    tain_t deadline;

    if(b) cenvp[0] = "BEVT_RELAY_RESTART=1" ;

    tain_now_g();
    tain_addsec_g(&deadline, 1);

    skaclient_startf_b_g(&bevt_client_g.connection, &bevt_client_g.buffers,
        cargv[0], cargv, cenvp,
        SKACLIENT_OPTION_WAITPID,
        BEVT_RELAY_BANNER1, BEVT_RELAY_BANNER1_LEN,
        BEVT_RELAY_BANNER2, BEVT_RELAY_BANNER2_LEN,
        &deadline);

    return (errno=0,0);
}


//*****************************************************************************
//*****************************************************************************
void bevt_client_handle_signals (void) {
    int wstat;

    for (;;) {
        BEVT_DEBUG_LOG_INFO("signal arrives");
        switch (selfpipe_read()) {
            case -1 : strerr_diefu1sys(111, "selfpipe_read") ;
            case 0 : return ;
            case SIGCHLD :
            {
                int r=wait_pid(bevt_client_g.connection.pid, &wstat);
                if(r==bevt_client_g.connection.pid) {
                    skaclient_end(&bevt_client_g.connection);
                    if(bevt_client_start_relay(1)<0) {
                        BEVT_DEBUG_LOG_ERROR("failed restart relay");
                    }
                }
                else {
                    BEVT_DEBUG_LOG_ERROR("pid differs (expected:%d, compared:%d)", bevt_client_g.connection.pid, r);
                }
            }
            return;

            default: break; 
        }
    }
}
