//
//  hashtable_iterator.c
//  hashtable
//
//  Created by Guanshan Liu on 01/08/2011.
//  Copyright 2011 Guanshan Liu. All rights reserved.
//
//  Based on Christopher Clark's hashtable
//

/*
 * Copyright (c) 2002, Christopher Clark
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 
 * * Neither the name of the original author; nor the names of any contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 * 
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdlib.h>
#include "hashtable_iterator.h"

hashtable_iterator *hashtable_iterator_create(hashtable *h) {
    unsigned int i, tablelength;
    hashtable_iterator *itr = (hashtable_iterator *)malloc(sizeof(hashtable_iterator));
    if (itr == NULL) 
        return NULL;
    itr->h = h;
    itr->e = NULL;
    itr->parent = NULL;
    tablelength = h->length;
    itr->index = tablelength;
    if (h->entrycount == 0) 
        return itr;
    
    for (i = 0; i < tablelength; i++)
    {
        if (h->table[i] != NULL)
        {
            itr->e = h->table[i];
            itr->index = i;
            break;
        }
    }
    return itr;
}

int hashtable_iterator_advance(hashtable_iterator *itr) {
    if (itr->e == NULL) 
        return 0;
    
    unsigned int j,tablelength;
    struct entry **table;
    struct entry *next;
    
    next = itr->e->next;
    if (next != NULL)
    {
        itr->parent = itr->e;
        itr->e = next;
        return -1;
    }
    tablelength = itr->h->length;
    itr->parent = NULL;
    if (tablelength <= (j = ++(itr->index)))
    {
        itr->e = NULL;
        return 0;
    }
    table = itr->h->table;
    while ((next = table[j]) == NULL)
    {
        if (++j >= tablelength)
        {
            itr->index = tablelength;
            itr->e = NULL;
            return 0;
        }
    }
    itr->index = j;
    itr->e = next;
    return -1;
}

int hashtable_iterator_remove(hashtable_iterator *itr) {
    entry *remember_e, *remember_parent;
    int ret;
    
    if ((itr->parent) == NULL)
    {
        itr->h->table[itr->index] = itr->e->next;
    } else {
        itr->parent->next = itr->e->next;
    }
    /* itr->e is now outside the hashtable */
    remember_e = itr->e;
    itr->h->entrycount--;
    free(remember_e->key);
    
    /* Advance the iterator, correcting the parent */
    remember_parent = itr->parent;
    ret = hashtable_iterator_advance(itr);
    if (itr->parent == remember_e) { 
        itr->parent = remember_parent; 
    }
    free(remember_e);
    return ret;
}

int hashtable_iterator_search(hashtable_iterator *itr, hashtable *h, void *k) {
    entry *e, *parent;
    unsigned int hashvalue, index;
    
    hashvalue = hash(h, k);
    index = indexFor(h->length, hashvalue);
    e = h->table[index];
    parent = NULL;
    while (e != NULL)
    {
        if ((hashvalue == e->hash) && (h->eqfn(k, e->key)))
        {
            itr->index = index;
            itr->e = e;
            itr->parent = parent;
            itr->h = h;
            return -1;
        }
        parent = e;
        e = e->next;
    }
    return 0;
}