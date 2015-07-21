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
 * \file        bevt_central_p.h
 * \brief       Message Management private APIs.
 * \version     0.1
 * \date        2013/01/14
 * \author      Vincent de RIBOU.
 * \copyright   Aquaplouf Land.
 *
 */

#ifndef _BEVT_CENTRAL_PRIV_H_
#define _BEVT_CENTRAL_PRIV_H_

/**
 * \ingroup BOZCORE_PRIV
 * \defgroup BOZCONNECT_PRIV Message Management private APIs.
 * \{
 */

#ifdef __cplusplus
extern "C"
{
#endif

#include <skalibs/unixmessage.h>
#include <skalibs/genset.h>
#include <skalibs/uint64.h>

#include <bozCore/bozclient.h>
#include <bozCore/boztree.h>

#include <bozEvent/bevt_client.h>
#include "bevt_debug_p.h"

#define BEVT_CENTRAL_BANNER1 "bevtcentral v1.0 (b)\n"
#define BEVT_CENTRAL_BANNER1_LEN (sizeof BEVT_CENTRAL_BANNER1 - 1)
#define BEVT_CENTRAL_BANNER2 "bevtcentral v1.0 (a)\n"
#define BEVT_CENTRAL_BANNER2_LEN (sizeof BEVT_CENTRAL_BANNER2 - 1)

#define BEVT_CENTRAL_STORAGE_REG_FLAG   (0x01)
#define BEVT_CENTRAL_STORAGE_SUB_FLAG   (0x10)

typedef struct {
    uint64          id;     /*!< identifier */ 
    uint64          nc;     /*!< notified count */ 
    unsigned char   rs;     /*!< registered/subscribed status */
} bevt_central_storage_t;
#define BEVT_CENTRAL_STORAGE_ID_INIT(id) { .id = id, .nc = 0, .rs = 0 }

typedef struct {
    bozmessage_receiver_t   in;         /*!< receiver of messages */
    bozmessage_sender_t     out;        /*!< sender of message */
    bozmessage_sender_t     asyncout;   /*!< sender of message */
    boztree_t               t;          /*!< link to registered/subscribed ids */
    unsigned int            xindex;     /*!< poll reverse index */
    char*                   d;          /*!< receiver data buffer */
} bevt_central_conn_t;

#define BEVT_CENTRAL_MAX_CONNS  10
typedef GENSETB_TYPE(bevt_central_conn_t, BEVT_CENTRAL_MAX_CONNS) bevt_central_rconns_t;
extern bevt_central_rconns_t bevt_relay_conns;

extern int main_socket_open(void);
extern int main_socket_accept(const int);
extern int main_socket_close(const int);

extern int bevt_central_parse_prot_cmd(bozmessage_t const *, void *);

/**
 *\}
 * End of group
 */

#ifdef __cplusplus
}
#endif

#endif   // _BEVT_CENTRAL_PRIV_H_ 
