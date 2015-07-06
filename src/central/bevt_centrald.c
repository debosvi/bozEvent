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
 * \file        bevt_centrald.c
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
#include <stdlib.h>
#include <errno.h>

#include <skalibs/sgetopt.h>
#include <skalibs/strerr2.h>
#include <skalibs/sig.h>
#include <skalibs/selfpipe.h>
#include <skalibs/tai.h>
#include <skalibs/djbunix.h>
#include <skalibs/iopause.h>

#include "bevt_central_p.h"

#define USAGE "bevt_centrald"
#define X() strerr_dief1x(101, "internal inconsistency, please submit a bug-report.")

static int mfd=-1;
static bevt_central_rconns_t relay_conns;

static int handle_close(bevt_central_conn_t *p) {
    boztree_free(&p->t);
    fd_close(bozmessage_receiver_fd(&p->r));
    free(p->d);
    bozmessage_receiver_free(&p->r);  
    bozmessage_sender_free(&p->s);  
    return (errno=0, 0);
}

static void cleanup (void) {
    register int i=0, n=gensetb_n(&relay_conns);
    
    for(; i<n; i++) {
        bevt_central_conn_t *p = gensetb_p(bevt_central_conn_t, &relay_conns, i);
        handle_close(p);
    }

    main_socket_close(mfd);
}

static int handle_accept(const int fd) {
    int r = main_socket_accept(fd);
    if(r<0) strerr_warnwu1x("unable to accept new connection");
    else {
        register int i = gensetb_new(&relay_conns);
        if(i<0) {
            close(r);
            return (errno=ENOMEM, -1);
        }
        bevt_central_conn_t *p = gensetb_p(bevt_central_conn_t, &relay_conns, i);
        BOZTREE_INIT(&p->t, bevt_central_storage_t);
        char *d = malloc(BEVT_MAX_DATA_SIZE);
        p->d = d;
        bozmessage_receiver_init(&p->r, r, d, BEVT_MAX_DATA_SIZE);  
        bozmessage_sender_init(&p->s, 16*BEVT_MAX_DATA_SIZE);  
    }
    return r;
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
    int sfd, no_df = 0 ;
    pid_t child;
    PROG = "bevt_centrald" ;

    if (argc < 1) strerr_dieusage(100, USAGE) ;
    if (ndelay_on(0) < 0) strerr_diefu2sys(111, "ndelay_on ", "0") ;
    if (ndelay_on(1) < 0) strerr_diefu2sys(111, "ndelay_on ", "1") ;
    if (sig_ignore(SIGPIPE) < 0) strerr_diefu1sys(111, "ignore SIGPIPE") ;


    {
        subgetopt_t l = SUBGETOPT_ZERO ;
        for (;;)
        {
            register int opt = subgetopt_r(argc, argv, "n", &l) ;
            if (opt == -1) break ;
            switch (opt)
            {
                case 'n' : no_df = 1; break ;
                default : strerr_dieusage(100, USAGE) ;
            }
        }
        argc -= l.ind ; argv += l.ind ;
    }

    if(!no_df) {
        child = doublefork();
        if (child < 0) strerr_diefu1sys(111, "doublefork") ;
        else if (child) strerr_die1(0, "success: ", "doublefork") ; 
    }

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

    GENSETB_init(bevt_central_conn_t, &relay_conns, BEVT_CENTRAL_MAX_CONNS);

    mfd = main_socket_open();
    if (mfd < 0) strerr_diefu1sys(111, "open_main_socket") ;

    tain_now_g() ;
    tain_addsec_g(&deadline, 2) ;

    for (;;) {
        register unsigned int n = gensetb_n(&relay_conns) ;
        iopause_fd x[2 + n] ;
        register int r ;
        unsigned int i=0;

        tain_add_g(&deadline, &tain_infinite_relative) ;
        x[0].fd = sfd ; x[0].events = IOPAUSE_READ ;
        x[1].fd = mfd ; x[1].events = IOPAUSE_READ ;

        for(; i<n; i++) {
            bevt_central_conn_t *p = gensetb_p(bevt_central_conn_t, &relay_conns, i);
            p->xindex = 2+i;
            x[2+i].fd = bozmessage_receiver_fd(&p->r);
            x[2+i].events = IOPAUSE_READ;
        }

        r = iopause_g(x, 2 + n, &deadline) ;
        if (r < 0) {
            cleanup() ;
            strerr_diefu1sys(111, "iopause") ;
        }

        /* signals arrived */
        if (x[0].revents & (IOPAUSE_READ | IOPAUSE_EXCEPT)) handle_signals() ;

        /* main socket arrived */
        if (x[1].revents & (IOPAUSE_READ | IOPAUSE_EXCEPT)) handle_accept(mfd) ;

        for (i=0 ; i<n; i++) {
            register bevt_central_conn_t *p = gensetb_p(bevt_central_conn_t, &relay_conns, i) ;
            if (x[p->xindex].revents & IOPAUSE_READ) {
                bozmessage_t m ;
                register int rr = sanitize_read(bozmessage_receive(&p->r, &m)) ;
                if (!rr) continue ;
                if (rr < 0) {
                    handle_close(p) ;
                    gensetb_delete(&relay_conns, i);
                }
            }
        }
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

