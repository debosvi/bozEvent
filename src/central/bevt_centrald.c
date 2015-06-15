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
 * \file        bevt_relayd.c
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
#include <signal.h>
#include <skalibs/allreadwrite.h>
#include <skalibs/error.h>
#include <skalibs/strerr2.h>
#include <skalibs/sig.h>
#include <skalibs/selfpipe.h>
#include <skalibs/tai.h>
#include <skalibs/djbunix.h>
#include <skalibs/iopause.h>
#include <skalibs/unixmessage.h>
#include <skalibs/skaclient.h>

#include "bevt_central_p.h"

#define USAGE "bevt_centrald"
#define X() strerr_dief1x(101, "internal inconsistency, please submit a bug-report.")

static void cleanup (void) {

}

static int handle_accept(int mfd) {
    return 0;
}

static void handle_signals (void) {
    for (;;) {
        switch (selfpipe_read()) {
            case -1 : cleanup() ; strerr_diefu1sys(111, "selfpipe_read") ;
            case 0 : return ;
            case SIGTERM :
            case SIGQUIT :
            case SIGHUP :
            case SIGABRT :
            case SIGINT : cleanup() ; _exit(0) ;
            case SIGCHLD : wait_reap() ; break ;
            default : cleanup() ; X() ;
        }
    }
}

int main (int argc, char const *const *argv) {
    tain_t deadline ;
    int sfd, mfd ;
    pid_t child;
    PROG = "bevt_centrald" ;

    if (argc < 1) strerr_dieusage(100, USAGE) ;
    if (ndelay_on(0) < 0) strerr_diefu2sys(111, "ndelay_on ", "0") ;
    if (ndelay_on(1) < 0) strerr_diefu2sys(111, "ndelay_on ", "1") ;
    if (sig_ignore(SIGPIPE) < 0) strerr_diefu1sys(111, "ignore SIGPIPE") ;

    child = doublefork();
    if (child < 0) strerr_diefu1sys(111, "doublefork") ;
    else if (child) strerr_die1(0, "success: ", "doublefork") ; 

    sfd = selfpipe_init() ;
    if (sfd < 0) strerr_diefu1sys(111, "selfpipe_init") ;
    {
        sigset_t set ;
        sigemptyset(&set) ;
        sigaddset(&set, SIGCHLD) ;
        sigaddset(&set, SIGTERM) ;
        sigaddset(&set, SIGQUIT) ;
        sigaddset(&set, SIGHUP) ;
        sigaddset(&set, SIGABRT) ;
        sigaddset(&set, SIGINT) ;
        if (selfpipe_trapset(&set) < 0)
            strerr_diefu1sys(111, "trap signals") ;
    }

    mfd = open_main_socket();
    if (mfd < 0) strerr_diefu1sys(111, "open_main_socket") ;

    tain_now_g() ;
    tain_addsec_g(&deadline, 2) ;

    for (;;) {
        register unsigned int n = 0 ;
        iopause_fd x[2 + n] ;
        int r ;

        tain_add_g(&deadline, &tain_infinite_relative) ;
        x[0].fd = sfd ; x[0].events = IOPAUSE_READ ;
        x[1].fd = mfd ; x[1].events = IOPAUSE_READ ;

        r = iopause_g(x, 2 + n, &deadline) ;
        if (r < 0) {
            cleanup() ;
            strerr_diefu1sys(111, "iopause") ;
        }

        /* signals arrived */
        if (x[0].revents & (IOPAUSE_READ | IOPAUSE_EXCEPT)) handle_signals() ;

        /* main socket arrived */
        if (x[1].revents & (IOPAUSE_READ | IOPAUSE_EXCEPT)) handle_accept(mfd) ;

//        /* client is writing */
//        if (!unixmessage_receiver_isempty(unixmessage_receiver_0) || x[0].revents & IOPAUSE_READ)
//        {
//            if (unixmessage_handle(unixmessage_receiver_0, &bevt_central_parse_prot_cmd, 0) < 0)
//            {
//                if (errno == EPIPE) break ; /* normal exit */
//                cleanup() ;
//                strerr_diefu1sys(111, "handle messages from client") ;
//            }
//        }

    }
    cleanup() ;
    return 0 ;
}

