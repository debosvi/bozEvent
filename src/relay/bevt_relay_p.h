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
 * \file        bevt_relay_p.h
 * \brief       Message Management private APIs.
 * \version     0.1
 * \date        2013/01/14
 * \author      Vincent de RIBOU.
 * \copyright   Aquaplouf Land.
 *
 */

#ifndef _BEVT_RELAY_PRIV_H_
#define _BEVT_RELAY_PRIV_H_

/**
 * \ingroup BOZCORE_PRIV
 * \defgroup BOZCONNECT_PRIV Message Management private APIs.
 * \{
 */

#ifdef __cplusplus
extern "C"
{
#endif

#include <sqlite3.h>

#include <skalibs/unixmessage.h>

#include <bozCore/bozmessage.h>
#include "bevt_debug_p.h"
#include "bozEvent/bevt_client.h"

#define BEVT_RELAY_PROG BEVT_PROG_PREFIX "bevt_relayd"
#define BEVT_RELAY_BANNER1 "bevtrelay v1.0 (b)\n"
#define BEVT_RELAY_BANNER1_LEN (sizeof BEVT_RELAY_BANNER1 - 1)
#define BEVT_RELAY_BANNER2 "bevtrelay v1.0 (a)\n"
#define BEVT_RELAY_BANNER2_LEN (sizeof BEVT_RELAY_BANNER2 - 1)

/**
 * @brief Relay private commands identifiers.
 */
typedef enum {
    BEVT_RELAY_OP_REGISTER_FIRST=0,
    BEVT_RELAY_OP_REGISTER_UPDATE,
    BEVT_RELAY_OP_UNREGISTER,
    BEVT_RELAY_OP_SUBSCRIBE_FIRST,
    BEVT_RELAY_OP_SUBSCRIBE_UPDATE,
    BEVT_RELAY_OP_UNSUBSCRIBE,
    BEVT_RELAY_OP_NOTIFY,
    BEVT_RELAY_OP_NB
} bevt_relay_op_t;
#define BEVT_RELAY_COMMAND_OP_LEN  (8)

extern const char* bevt_relay_commands[BEVT_RELAY_OP_NB];

extern int bevt_relay_db_init(char const *db_name);
extern int bevt_relay_db_close(void);

extern int bevt_relay_parse_prot_cmd(unixmessage_t const *m, void *context);
extern int bevt_relay_parse_prot_event(unixmessage_t const *m, void *context);
  
int main_socket_open(void);
int main_socket_close(const int);

extern sqlite3 *bevt_relayd_db_g;
extern bozmessage_receiver_t central_receive;
extern bozmessage_sender_t central_sender;

/**
 *\}
 * End of group
 */

#ifdef __cplusplus
}
#endif

#endif   // _BEVT_RELAY_PRIV_H_ 
