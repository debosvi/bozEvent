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
#include "skalibs/selfpipe.h"
#include "skalibs/strerr2.h"

#include "bevt_client_p.h"

//*****************************************************************************
//*****************************************************************************
int bevt_client_init(void) {
    random_name(&bevt_client_g.rid[0], 8);

    bevt_client_g.sfd = selfpipe_init() ;
    if (bevt_client_g.sfd < 0) strerr_diefu1sys(111, "selfpipe_init") ;
    {
        sigset_t set ;
        sigemptyset(&set) ;
        sigaddset(&set, SIGCHLD) ;
        if (selfpipe_trapset(&set) < 0)
            strerr_diefu1sys(111, "trap signals") ;
    }

    BEVT_DEBUG_LOG_INFO("rid(%s)", (long long int)bevt_client_g.rid);
    if(bevt_client_start_relay(0)<0)
        return -1;

    return (errno=0,0);
}

