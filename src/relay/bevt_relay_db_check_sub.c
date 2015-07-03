
#include <stdio.h>

#include "bevt_relay_p.h"

static const char* bevt_relay_db_sub_sql = "SELECT count(id) FROM credentials where id == %d and subscribe != 0;";

int bevt_relay_db_check_sub(const bevt_client_id_t id) {
    int r;
    int len = UINT64_FMT+strlen(bevt_relay_db_sub_sql);
    char sql_req[len];

    snprintf(sql_req, len, bevt_relay_db_sub_sql, id);

    r = sqlite3_exec(bevt_relayd_db_g, sql_req, 0, 0, 0);
    if(r<0)
        return -1;

    return 0;
}
