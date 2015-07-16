
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include <errno.h>

#include "skalibs/skamisc.h"
#include "bevt_relay_p.h"

int main(void) {
    int ret=0, i=0;
    bevt_relay_db_elem_t elem;

    ret=bevt_relay_db_init("test.sqlite");
    if(ret)
        fprintf(stderr, "bevt_relay_db_init failed, errno(%d/%s)\n", errno, strerror(errno));


    while(i++<(10*60)) {
        fprintf(stderr, "\nLOOP %d\n", i);
        ret=bevt_relay_db_get_elem(i, &elem);
        if(ret<0)
            fprintf(stderr, "bevt_relay_db_get_elem: FAILURE\n");
        else if(!ret) {
            fprintf(stderr, "bevt_relay_db_get_elem: OK\n");
            fprintf(stderr, "\tid(%llu)\n", (long long unsigned int)elem.id);
            fprintf(stderr, "\tregister(%d)\n", elem.reg);
            fprintf(stderr, "\tsubscribe(%d)\n", elem.sub);
            fprintf(stderr, "\tnotify(%llu)\n", (long long unsigned int)elem.nt);
            fprintf(stderr, "\tpriority(%d)\n", elem.prio);
        }

        ret=bevt_relay_db_check_reg(i);

        if(ret<0)
            fprintf(stderr, "bevt_relay_db_check_reg: FAILURE\n");
        else if(!ret) {
            fprintf(stderr, "bevt_relay_db_check_reg: NOK\n");
        }
        else {
            fprintf(stderr, "bevt_relay_db_check_reg: SUCCESS\n");
        }

        elem.reg^=0x01;
        ret=bevt_relay_db_set_elem(&elem);
        if(ret<0)
            fprintf(stderr, "bevt_relay_db_set_elem: FAILURE\n");
        else
            fprintf(stderr, "bevt_relay_db_set_elem: SUCCESS\n");

        ret=bevt_relay_db_check_sub(i);

        if(ret<0)
            fprintf(stderr, "bevt_relay_db_check_sub: FAILURE\n");
        else if(!ret)
            fprintf(stderr, "bevt_relay_db_check_sub: NOK\n");
        else
            fprintf(stderr, "bevt_relay_db_check_sub: SUCCESS\n");
        //bevt_client_process(100);
        //poll(0,0, 1000);
    };

    ret=bevt_relay_db_close();
    if(ret)
        fprintf(stderr, "bevt_relay_db_close failed, errno(%d/%s)\n", errno, strerror(errno));

    stralloc_free(&satmp);
    exit(EXIT_SUCCESS);
}
