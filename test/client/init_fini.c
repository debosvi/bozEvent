
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "bozEvent/bevt_client.h"

int main(int ac, char **av) {
    int ret=0, i=0;

    ret=bevt_client_init();
    if(ret)
        fprintf(stderr, "bevt_client_init failed, errno(%d/%s)\n", errno, strerror(errno));

    while(i++<(10*60)) {
        bevt_client_process(100);
    };

    ret=bevt_client_finalise();
    if(ret)
        fprintf(stderr, "bevt_client_finalise failed, errno(%d/%s)\n", errno, strerror(errno));

    exit(EXIT_SUCCESS);
}
