
#include <stdio.h>

#include "bevt_relay_p.h"

int bevt_relay_db_check_reg(const bevt_client_id_t id) {
    int r;
    bevt_relay_db_elem_t elem;

    r = bevt_relay_db_get_elem(id, &elem);
    if(r<0)
        return 0;

    return (id==elem.id)&&(elem.reg);
}
