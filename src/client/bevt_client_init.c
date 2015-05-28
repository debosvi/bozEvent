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
 * \file        bevt_client_init.c
 * \brief       Message creation implementation.
 * \version     0.1
 * \date        2013/01/14
 * \author      Vincent de RIBOU.
 * \copyright   Aquaplouf Land.
 *
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "skalibs/random.h"
#include "skalibs/environ.h"
#include "bevt_client_p.h"

//*****************************************************************************
//*****************************************************************************
static int bevt_client_start_relay(void) {
    char rid_str[UINT64_FMT];
    char const *cargv[3] = { BEVT_RELAY_PROG, &rid_str[0], 0 } ;
    tain_t deadline;

    tain_now_g();
    tain_addsec_g(&deadline, 1);
    uint64_fmt(rid_str, bevt_client_g.rid);

    skaclient_startf_b_g(&bevt_client_g.connection, &bevt_client_g.buffers, 
        cargv[0], cargv, (char const *const *)environ,
        SKACLIENT_OPTION_WAITPID,
        BEVT_RELAY_BANNER1, BEVT_RELAY_BANNER1_LEN, 
        BEVT_RELAY_BANNER2, BEVT_RELAY_BANNER2_LEN,
        &deadline);
    return (errno=0,0);
}

//*****************************************************************************
//*****************************************************************************
int bevt_client_init(void) {
    badrandom_init() ;
    bevt_client_g.rid = badrandom_int(INT32_MAX);
    badrandom_finish() ;

    fprintf(stderr, "%s: rid(%lld)\n", __PRETTY_FUNCTION__, (long long int)bevt_client_g.rid);
    if(bevt_client_start_relay()<0)
        return -1;

    return (errno=0,0);
}

