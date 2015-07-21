
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include <errno.h>

#include "skalibs/strerr2.h"
#include "skalibs/tai.h"
#include "skalibs/iopause.h"
#include <skalibs/sgetopt.h>

#include "bozEvent/bevt_client.h"

#define USAGE PROGNAME" <-r> <-u> <-s> <-v>"

//******************************************************************************
//******************************************************************************
static void do_register(const bevt_client_id_t id, const char update) {
    int ret;
    if(!update)
        ret=bevt_client_register(id, BEVT_CLIENT_PRIO_DEFAULT);
    else
        ret=bevt_client_register_update(id, BEVT_CLIENT_PRIO_MAJOR);
    if(ret<0)
        fprintf(stderr, "bevt_client_register failed 1, (long long int)id(%lld), errno(%d/%s)\n", (long long int)id, errno, strerror(errno));
    else
        fprintf(stderr, "bevt_client_register success, (long long int)id(%lld)\n", (long long int)id);
}


//******************************************************************************
//******************************************************************************
static void do_unregister(const bevt_client_id_t id) {
    int ret = bevt_client_unregister(id);
    if(ret<0)
        fprintf(stderr, "bevt_client_unregister failed 1, (long long int)id(%lld), errno(%d/%s)\n", (long long int)id, errno, strerror(errno));
    else
        fprintf(stderr, "bevt_client_unregister success, (long long int)id(%lld)\n", (long long int)id);
}

//******************************************************************************
//******************************************************************************
static void do_subscribe(const bevt_client_id_t id, const char update) {
    int ret;
    if(!update)
        ret=bevt_client_subscribe(id, BEVT_CLIENT_PRIO_DEFAULT);
    else
        ret=bevt_client_subscribe_update(id, BEVT_CLIENT_PRIO_MAJOR);
    if(ret<0)
        fprintf(stderr, "bevt_client_subscribe failed 1, (long long int)id(%lld), errno(%d/%s)\n", (long long int)id, errno, strerror(errno));
    else
        fprintf(stderr, "bevt_client_subscribe success, (long long int)id(%lld)\n", (long long int)id);
}

//******************************************************************************
//******************************************************************************
static void do_unsubscribe(const bevt_client_id_t id) {
    int ret = bevt_client_unsubscribe(id);
    if(ret<0)
        fprintf(stderr, "bevt_client_unsubscribe failed 1, (long long int)id(%lld), errno(%d/%s)\n", (long long int)id, errno, strerror(errno));
    else
        fprintf(stderr, "bevt_client_unsubscribe success, (long long int)id(%lld)\n", (long long int)id);
}

//******************************************************************************
//******************************************************************************
int main(int argc, char const *const *argv) {
    int ret=0;
    int action=-1;
    int update=0;
    bevt_client_id_t id=0xFFFFFFFF;

    ret=bevt_client_init();
    if(ret)
        fprintf(stderr, "bevt_client_init failed, errno(%d/%s)\n", errno, strerror(errno));

    {
        subgetopt_t l = SUBGETOPT_ZERO ;
        for (;;)
        {
            register int opt = subgetopt_r(argc, argv, "ur:v:s:w:p", &l) ;
            if (opt == -1) break ;
            switch (opt)
            {
                case 'u' : update=1; continue ;
                case 'r' : action=0; id=atoi(l.arg); break ;
                case 'v' : action=1; id=atoi(l.arg); break ;
                case 's' : action=2; id=atoi(l.arg); break ;
                case 'w' : action=3; id=atoi(l.arg); break ;
                case 'p' : action=4; break ;
                default : strerr_dieusage(100, USAGE) ;
            }

            switch(action) {
            case 0: do_register(id, update); break;
            case 1: do_unregister(id); break;
            case 2: do_subscribe(id, update); break;
            case 3: do_unsubscribe(id); break;
            case 4: getchar(); break;
            default:
                fprintf(stderr, "parsed action failed(%d)\n", action);
            }

            update=0; 
            action=-1;
            id=0xFFFFFFFF;

        }
        argc -= l.ind ; argv += l.ind ;
    }

    ret=bevt_client_finalise();
    if(ret)
        fprintf(stderr, "bevt_client_finalise failed, errno(%d/%s)\n", errno, strerror(errno));

    exit(EXIT_SUCCESS);
}
