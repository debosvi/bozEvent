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
 * \file        bevt_client_p.h
 * \brief       Message Management private APIs.
 * \version     0.1
 * \date        2013/01/14
 * \author      Vincent de RIBOU.
 * \copyright   Aquaplouf Land.
 *
 */

#ifndef _BEVT_CLIENT_PRIV_H_
#define _BEVT_CLIENT_PRIV_H_

/**
 * \ingroup BOZCORE_PRIV
 * \defgroup BOZCONNECT_PRIV Message Management private APIs.
 * \{
 */

#ifdef __cplusplus
extern "C"
{
#endif

#include <skalibs/skaclient.h>
#include <skalibs/buffer.h>

#include "bozEvent/bevt_client.h"

#define BEVT_RELAY_PROG BEVT_PROG_PREFIX "bevt_relayd"
#define BEVT_RELAY_BANNER1 "bevtrelay v1.0 (b)\n"
#define BEVT_RELAY_BANNER1_LEN (sizeof BEVT_RELAY_BANNER1 - 1)
#define BEVT_RELAY_BANNER2 "bevtrelay v1.0 (a)\n"
#define BEVT_RELAY_BANNER2_LEN (sizeof BEVT_RELAY_BANNER2 - 1)
  
/**
 * @brief Messsage global type.
 */
typedef struct {
    uint64_t    rid;      /*!< Random identifier */
    skaclient_t connection ;
    skaclient_buffer_t buffers ;
} bevt_client_glob_t;

/**
 * @brief Messsage global type initializer.
 */
#define BEVT_CLIENT_GLOB_ZERO {     \
    .rid=0,                         \
    .connection = SKACLIENT_ZERO,   \
}

/**
 * @brief Messsage global type instance.
 */
extern bevt_client_glob_t bevt_client_g;

/**
 *\}
 * End of group
 */

#ifdef __cplusplus
}
#endif

#endif   // _BEVT_CLIENT_PRIV_H_ 