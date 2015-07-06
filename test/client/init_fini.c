
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include <errno.h>

#include "bozEvent/bevt_client.h"

int main(void) {
    int ret=0, i=0;

    ret=bevt_client_init();
    if(ret)
        fprintf(stderr, "bevt_client_init failed, errno(%d/%s)\n", errno, strerror(errno));

    while(i++<(10*60)) {
        ret = bevt_client_register(i, BEVT_CLIENT_PRIO_DEFAULT);
        if(ret<0)
            fprintf(stderr, "bevt_client_register failed 1, id(%d), errno(%d/%s)\n", i, errno, strerror(errno));

        ret = bevt_client_register(i, BEVT_CLIENT_PRIO_DEFAULT);
        if(ret<0)
            fprintf(stderr, "bevt_client_register failed 2, id(%d), errno(%d/%s)\n", i, errno, strerror(errno));

        ret = bevt_client_unregister(i);
        if(ret<0)
            fprintf(stderr, "bevt_client_unregister failed 1, id(%d), errno(%d/%s)\n", i, errno, strerror(errno));

        ret = bevt_client_unregister(i);
        if(ret<0)
            fprintf(stderr, "bevt_client_unregister failed 2, id(%d), errno(%d/%s)\n", i, errno, strerror(errno));

        ret = bevt_client_register(i, BEVT_CLIENT_PRIO_DEFAULT);
        if(ret<0)
            fprintf(stderr, "bevt_client_register failed 2, id(%d), errno(%d/%s)\n", i, errno, strerror(errno));
//        poll(0,0, 100);
        bevt_client_process(100);
    };

    ret=bevt_client_finalise();
    if(ret)
        fprintf(stderr, "bevt_client_finalise failed, errno(%d/%s)\n", errno, strerror(errno));

    exit(EXIT_SUCCESS);
}
