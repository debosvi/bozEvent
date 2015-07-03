
#include <stdio.h>

#include "bevt_relay_p.h"

static const char* bevt_relay_db_reg_sql = "SELECT count(id) FROM credentials where id == %d and register != 0;";

static int bevt_relay_db_reg_cb(void *data, int argc, char **argv, char **azColName){
   int i;
   for(i=0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
    (*(unsigned char*)data) = (*argv[0]=='0' ? 0 : 1);
   return 0;
}


int bevt_relay_db_check_reg(const bevt_client_id_t id) {
    int r;
    int len = UINT64_FMT+strlen(bevt_relay_db_reg_sql);
    char sql_req[len];
    unsigned char count;

    snprintf(sql_req, len, bevt_relay_db_reg_sql, id);    

    r = sqlite3_exec(bevt_relayd_db_g, sql_req, bevt_relay_db_reg_cb, &count, 0);
    if(r<0)
        return -1;

    return (count ? 1 : 0);
}
