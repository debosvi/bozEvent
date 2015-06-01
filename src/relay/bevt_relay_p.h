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

#include <skalibs/unixmessage.h>

#include "bevt_debug_p.h"

#define BEVT_RELAY_PROG BEVT_PROG_PREFIX "bevt_relayd"
#define BEVT_RELAY_BANNER1 "bevtrelay v1.0 (b)\n"
#define BEVT_RELAY_BANNER1_LEN (sizeof BEVT_RELAY_BANNER1 - 1)
#define BEVT_RELAY_BANNER2 "bevtrelay v1.0 (a)\n"
#define BEVT_RELAY_BANNER2_LEN (sizeof BEVT_RELAY_BANNER2 - 1)

extern int bevt_relay_db_init(char const *db_name);
extern int bevt_relay_parse_prot_cmd(unixmessage_t const *m, void *context);
extern int bevt_relay_parse_prot_event(unixmessage_t const *m, void *context);
  
/**
 *\}
 * End of group
 */

#ifdef __cplusplus
}
#endif

#endif   // _BEVT_RELAY_PRIV_H_ 
