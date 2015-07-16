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
 * \file        bevt_relay_db_get_elem.c
 * \brief       BozEvent relay client executable.
 * \version     0.1.0
 * \date        2013/01/14
 * \author      Vincent de RIBOU.
 * \copyright   Aquaplouf Land.
 *
 * \brief Implements client relay to central node.
 *
 */

#include <stdlib.h>

#include "bevt_relay_p.h"

static int parse_line(const bevt_client_id_t id, const char * const s, bevt_relay_db_elem_t* const elem) {
    register char *p;
    register char *base;
    uint64 u;
    int i;

    if(s[0]=='#') return 0;

    base = (char*)s;

    p = strchr(base,':');
    if(!p) return 0;

    *p = 0;
    uint64_scan(base, &u);
    if(u!=id) return 0;
    
    elem->id = u;
    
    base = p+1;
    p = strchr(base,':');
    *p = 0;
    i = atoi(base);
    elem->reg = (i?1:0);
    
    base = p+1;
    p = strchr(base,':');
    *p = 0;
    i = atoi(base);
    elem->sub = (i?1:0);
    
    base = p+1;
    p = strchr(base,':');
    *p = 0;
    i = atoi(base);
    elem->prio = i;
    
    base = p+1;
    uint64_scan(base, &elem->nt);

    return 1;
}

int bevt_relay_db_get_elem(const bevt_client_id_t id, bevt_relay_db_elem_t* const elem) {
    int ret=0;
    stralloc data = STRALLOC_ZERO;
    register char *base;

    if(!id) { errno=ENOENT; ret=-1; goto exit; }

    if(openreadclose(bevt_relay_db_name_g.s, &data, 0)<0 ) { ret=-1; goto exit; }

    base = data.s;
    while(1) {
        char *p = strchr(base, 0x0a);
        if(p) (*p) = 0;
        if(parse_line(id, base, elem)) break;
        if(!p) { ret=-1; errno = ENOENT; break; }
        base = p+1;
    }

exit:
    stralloc_free(&data);
    if(!ret) errno=0;
    else memset(elem, 0, sizeof(bevt_relay_db_elem_t));
    return ret;
}
