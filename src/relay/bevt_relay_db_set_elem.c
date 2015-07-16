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
 * \file        bevt_relay_db_set_elem.c
 * \brief       BozEvent relay client executable.
 * \version     0.1.0
 * \date        2013/01/14
 * \author      Vincent de RIBOU.
 * \copyright   Aquaplouf Land.
 *
 * \brief Implements client relay to central node.
 *
 */

#include <stdio.h>

#include "bevt_relay_p.h"

//******************************************************************************
//******************************************************************************
static void remove_line(stralloc *sa, const bevt_client_id_t id) {
    uint64 u;
    char aux[64];
    register char *base;
    int len;

    base = sa->s;
    while(1) {
        char *p;

        memset(aux, 0, 64);
        snprintf(aux, 64, "%s", base);
        p = strchr(base, 0x0a);

        if(strlen(aux) && aux[0]!='#') {
            char *pp = strchr(aux, ':');
            *pp = 0;
            uint64_scan(aux, &u);
            if(u==id) {
                if(p) {
                    len = p+1-base;
                    unsigned int s = (sa->s+sa->len)-(base+len);
                    memmove(base, p+1, s);
                }                
                else len = (sa->s+sa->len)+1 - base;
                sa->len-=len;
                sa->s[sa->len]=0;
                continue;
            }
        }
        if(!p) break;
        base = p+1;
    }
}

//******************************************************************************
//******************************************************************************
static void prepare_line(stralloc *sa, bevt_relay_db_elem_t const * const elem) {
    int len=0;

    sa->len=0;
    stralloc_ready(sa, 64);
    memset(sa->s, 0, sa->a);
    len = uint64_fmt(&sa->s[sa->len], elem->id);
    sa->len += len;
    stralloc_append(sa, ':');
    sa->s[sa->len++] = (elem->reg?'1':'0');
    stralloc_append(sa, ':');
    sa->s[sa->len++] = (elem->sub?'1':'0');
    stralloc_append(sa, ':');
    sa->s[sa->len++] = '0' + elem->prio;
    stralloc_append(sa, ':');
    len = uint64_fmt(&sa->s[sa->len], elem->nt);
    sa->len += len;
    stralloc_append(sa, '\n');
}

//******************************************************************************
//******************************************************************************
int bevt_relay_db_set_elem(bevt_relay_db_elem_t const * const elem) {
    int ret, updt=0;
    stralloc data = STRALLOC_ZERO;
    stralloc aux = STRALLOC_ZERO;
    bevt_relay_db_elem_t telem;

    if(!elem->id) return (errno=EINVAL,-1);

    if(openreadclose(bevt_relay_db_name_g.s, &data, 0)<0 ) return -1;
    data.s[data.len]=0;

    while (1) {
        ret = bevt_relay_db_get_elem(elem->id, &telem);
        if(ret<0) {
            /* cretae elem */
            telem.id = elem->id;
            prepare_line(&aux, &telem);
            stralloc_cat(&data, &aux);
            openwritenclose(bevt_relay_db_name_g.s, data.s, data.len);
            continue;
        }
        else {
            if(telem.reg != elem->reg)      { telem.reg  = elem->reg;  updt++; }
            if(telem.sub != elem->sub)      { telem.sub  = elem->sub;  updt++; }
            if(telem.nt < elem->nt)         { telem.nt   = elem->nt;   updt++; }
            if(telem.prio != elem->prio)    { telem.prio = elem->prio; updt++; }
            if(updt) {
                prepare_line(&aux, &telem);
                remove_line(&data, elem->id);
                stralloc_cat(&data, &aux);
                openwritenclose(bevt_relay_db_name_g.s, data.s, data.len);
            }   
            break;
        }
    }

    stralloc_free(&data);
    stralloc_free(&aux);
    if(!ret) errno=0;
    return ret;
}
