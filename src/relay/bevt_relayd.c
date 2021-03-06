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

#include "bevt_relay_p.h"

#define USAGE "bevt_relayd <random db name>"
#define X() strerr_dief1x(101, "internal inconsistency, please submit a bug-report.")

static int mfd=-1;

static void cleanup (void) {
    main_socket_close(mfd);
    bevt_relay_db_close();
}

static int handle_connect_central(void) {
    tain_t deadline;
    mfd = main_socket_open();
    if (mfd < 0) { 
        strerr_warnw1x("open_main_socket") ; 
        return -1; 
    }
    tain_now_g();
    tain_addsec_g(&deadline, 1);
    bozclient_start_b_g(&central_client_g, &central_client_data_g, mfd, 
        BEVT_CENTRAL_BANNER1, BEVT_CENTRAL_BANNER1_LEN,
        BEVT_CENTRAL_BANNER2, BEVT_CENTRAL_BANNER2_LEN,
        &deadline);
    return 0;
}

static int handle_close_central(void) {
    close(mfd); 
    mfd=-1;
    bozclient_end(&central_client_g);
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
    int sfd ;
    PROG = "bevt_relayd" ;

    if (argc < 2) strerr_dieusage(100, USAGE) ;
    if (ndelay_on(0) < 0) strerr_diefu2sys(111, "ndelay_on ", "0") ;
    if (ndelay_on(1) < 0) strerr_diefu2sys(111, "ndelay_on ", "1") ;
    if (sig_ignore(SIGPIPE) < 0) strerr_diefu1sys(111, "ignore SIGPIPE") ;

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

    if(bevt_relay_db_init(argv[1])<0)
        strerr_diefu1sys(111, "db init failed") ;

    tain_now_g() ;
    tain_addsec_g(&deadline, 2) ;

    if (!skaclient_server_01x_init_g(BEVT_RELAY_BANNER1, BEVT_RELAY_BANNER1_LEN, BEVT_RELAY_BANNER2, BEVT_RELAY_BANNER2_LEN, &deadline))
        strerr_diefu1sys(111, "sync with client") ;

    for (;;) {
        register unsigned int n = 0 ;
        iopause_fd x[6 + n] ;
        int r ;

        if(mfd<0) handle_connect_central();

        tain_add_g(&deadline, &tain_infinite_relative) ;
        x[0].fd = 0 ; x[0].events = IOPAUSE_EXCEPT | IOPAUSE_READ ;
        x[1].fd = 1 ; x[1].events = IOPAUSE_EXCEPT | (unixmessage_sender_isempty(unixmessage_sender_1) ? 0 : IOPAUSE_WRITE ) ;
        x[2].fd = sfd ; x[2].events = IOPAUSE_READ ;
        x[3].fd = bozclient_sfd(&central_client_g); x[3].events = IOPAUSE_READ ;
        x[4].fd = bozclient_sfd(&central_client_g); x[4].events = (bozclient_siswritable(&central_client_g) ? IOPAUSE_WRITE : 0) ;
        x[5].fd = unixmessage_sender_fd(unixmessage_sender_x) ; x[5].events = (unixmessage_sender_isempty(unixmessage_sender_x) ? 0 : IOPAUSE_WRITE)  ;

        r = iopause_g(x, 5 + n, &deadline) ;
        if (r < 0) {
            cleanup() ;
            strerr_diefu1sys(111, "iopause") ;
        }

        /* client closed */
        if ((x[0].revents | x[1].revents) & IOPAUSE_EXCEPT) break ;

        /* client is sync reading */
        if (x[1].revents & IOPAUSE_WRITE) {
            if (!unixmessage_sender_flush(unixmessage_sender_1) && !error_isagain(errno)) {
                cleanup() ;
                strerr_diefu1sys(111, "flush sync out") ;
            }
        }

        /* client is async rzading */
        if (x[5].revents & IOPAUSE_WRITE) {
            if (!unixmessage_sender_flush(unixmessage_sender_x) && !error_isagain(errno)) {
                cleanup() ;
                strerr_diefu1sys(111, "flush async out") ;
            }
        }

        /* signals arrived */
        if (x[2].revents & (IOPAUSE_READ | IOPAUSE_EXCEPT)) handle_signals() ;

        /* main socket read or close */
        if (x[3].revents & IOPAUSE_READ) {
            handle_close_central();
            continue;
        }

        /* main socket close */
        if (x[4].revents & IOPAUSE_WRITE) {
            bozclient_flush(&central_client_g);
        }

        /* main socket close */
        if (x[5].revents & IOPAUSE_WRITE) {
            if (!unixmessage_sender_flush(unixmessage_sender_x) && !error_isagain(errno)) {
                cleanup() ;
                strerr_diefu1sys(111, "flush stdout") ;
            }
        }

        /* client is sync writing */
        if (!unixmessage_receiver_isempty(unixmessage_receiver_0) || x[0].revents & IOPAUSE_READ)
        {
            if (unixmessage_handle(unixmessage_receiver_0, &bevt_relay_parse_prot_cmd, 0) < 0)
            {
                if (errno == EPIPE) break ; /* normal exit */
                cleanup() ;
                strerr_diefu1sys(111, "handle messages from client") ;
            }
        }
    }
    cleanup() ;
    return 0 ;
}

