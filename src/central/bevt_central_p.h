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

#include <bozCore/bozmessage.h>
#include <bozCore/boztree.h>

#include <bozEvent/bevt_client.h>
#include "bevt_debug_p.h"

typedef struct {
    uint64          nc;     /*!< notified count */ 
    unsigned char   rs;     /*!< registered/subscribed status */
} bevt_central_storage_t;

typedef struct {
    bozmessage_receiver_t   r;          /*!< receiver of messages */
    bozmessage_sender_t     s;          /*!< sender of message */
    boztree_t               t;          /*!< link to registered/subscribed ids */
    unsigned int            xindex;     /*!< poll reverse index */
    char*                   d;          /*!< receiver data buffer */
} bevt_central_conn_t;

#define BEVT_CENTRAL_MAX_CONNS  10
typedef GENSETB_TYPE(bevt_central_conn_t, BEVT_CENTRAL_MAX_CONNS) bevt_central_rconns_t;

int main_socket_open(void);
int main_socket_accept(const int);
int main_socket_close(const int);

/**
 *\}
 * End of group
 */

#ifdef __cplusplus
}
#endif

#endif   // _BEVT_CENTRAL_PRIV_H_ 
