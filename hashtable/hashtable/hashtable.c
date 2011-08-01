//
//  hashtable.c
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "hashtable.h"

/*
 Credit for primes table: Aaron Krowne
 http://br.endernet.org/~akrowne/
 http://planetmath.org/encyclopedia/GoodHashTablePrimes.html
 */
static const unsigned int primes[] = {
    53, 97, 193, 389,
    769, 1543, 3079, 6151,
    12289, 24593, 49157, 98317,
    196613, 393241, 786433, 1572869,
    3145739, 6291469, 12582917, 25165843,
    50331653, 100663319, 201326611, 402653189,
    805306457, 1610612741
};

const unsigned int prime_table_length = sizeof(primes)/sizeof(primes[0]);
const float max_load_factor = 0.65f;

unsigned int hash(hashtable *h, void *k) {
    /* Aim to protect against poor hash functions by adding logic here
     * - logic taken from java 1.4 hashtable source */
    unsigned int i = h->hashfn(k);
    i += ~(i << 9);
    i ^=  ((i >> 14) | (i << 18)); /* >>> */
    i +=  (i << 4);
    i ^=  ((i >> 10) | (i << 22)); /* >>> */
    return i;
}

hashtable* hashtable_create(unsigned int minsize, unsigned int (*hashfunction)(void*), int (*key_eq_fn)(void*, void*)) {
    if (minsize > (1u << 30)) {
        return NULL; // too large
    }
    
    hashtable *h;
    unsigned int pindex;
    unsigned int size;
    // force size as prime
    for (pindex = 0; pindex < prime_table_length; pindex++) {
        if (primes[pindex] > minsize) {
            size = primes[pindex];
            break;
        }
    }
    h = (hashtable *)malloc(sizeof(hashtable));
    if (h == NULL) {
        return NULL;
    }
    h->table = (entry **)malloc(sizeof(entry *) * size);
    if (h->table == NULL) {
        free(h);
        return NULL;
    }
    memset(h->table, 0, sizeof(entry *) * size);
    h->length = size;
    h->primeindex = pindex;
    h->entrycount = 0;
    h->hashfn = hashfunction;
    h->eqfn = key_eq_fn;
    h->loadlimit = (unsigned int)ceil(size * max_load_factor);
    return h;
}

static int hashtable_expand(hashtable *h) {
    struct entry **newtable;
    struct entry *e;
    struct entry **pE;
    unsigned int newsize, i, index;
    /* Check we're not hitting max capacity */
    // disable: invalid data...
#pragma warning(disable:6385)
    if (h->primeindex == (prime_table_length - 1)) return 0;
    newsize = primes[++(h->primeindex)];
    
    newtable = (struct entry **)malloc(sizeof(struct entry*) * newsize);
    if (newtable != NULL)
    {
        memset(newtable, 0, newsize * sizeof(struct entry *));
        for (i = 0; i < h->length; i++) {
            while ((e = h->table[i]) != NULL) {
                h->table[i] = e->next;
                index = indexFor(newsize,e->hash);
                e->next = newtable[index];
                newtable[index] = e;
            }
        }
        free(h->table);
        h->table = newtable;
    }
    /* Plan B: realloc instead */
    else 
    {
        newtable = (struct entry **)realloc(h->table, newsize * sizeof(struct entry *));
        if (newtable == NULL) { 
            (h->primeindex)--;
            return 0; 
        }
        h->table = newtable;
        memset(newtable[h->length], 0, newsize - h->length);
        for (i = 0; i < h->length; i++) {
            for (pE = &(newtable[i]), e = *pE; e != NULL; e = *pE) {
                index = indexFor(newsize,e->hash);
                *pE = e->next;
                if (index != i) //== i)
                {
                    //pE = &(e->next);
                //}
                //else
                //{
                    //*pE = e->next;
                    e->next = newtable[index];
                    newtable[index] = e;
                }
            }
        }
    }
    h->length = newsize;
    h->loadlimit = (unsigned int)ceil(newsize * max_load_factor);
    return -1;
}

int hashtable_set(hashtable *h, void *k, void *v) {
    struct entry *e;
    unsigned int hashvalue, index;
    hashvalue = hash(h,k);
    index = indexFor(h->length,hashvalue);
    e = h->table[index];
    while (e != NULL)
    {
        /* Check hash value to short circuit heavier comparison */
        if ((hashvalue == e->hash) && (h->eqfn(k, e->key)))
        {
            free(e->value);
            e->value = v;
            return -1;
        }
        e = e->next;
    }
    
    if (++(h->entrycount) > h->loadlimit) {
        hashtable_expand(h);
    }
    e = (struct entry *)malloc(sizeof(struct entry));
    if (e == NULL) { 
        --(h->entrycount); 
        return 0; 
    }
    e->hash = hash(h,k);
    index = indexFor(h->length, e->hash);
    e->key = k;
    e->value = v;
    e->next = h->table[index];
    h->table[index] = e;
    return -1;
}

void* hashtable_get(hashtable *h, void *k) {
    struct entry *e;
    unsigned int hashvalue, index;
    hashvalue = hash(h, k);
    index = indexFor(h->length,hashvalue);
    e = h->table[index];
    while (e != NULL) {
        /* Check hash value to short circuit heavier comparison */
        if ((hashvalue == e->hash) && (h->eqfn(k, e->key))) 
            return e->value;
        e = e->next;
    }
    return NULL;
}

void* hashtable_remove(hashtable *h, void *k) {
    // TODO: consider compacting the table when the load factor drops enough,
    //       or provide a 'compact' method.
    
    struct entry *e;
    struct entry **pE;
    void *v;
    unsigned int hashvalue, index;
    
    hashvalue = hash(h,k);
    index = indexFor(h->length, hash(h, k));
    pE = &(h->table[index]);
    e = *pE;
    while (e != NULL) {
        /* Check hash value to short circuit heavier comparison */
        if ((hashvalue == e->hash) && (h->eqfn(k, e->key)))
        {
            *pE = e->next;
            h->entrycount--;
            v = e->value;
            free(e->key);
            free(e);
            return v;
        }
        pE = &(e->next);
        e = e->next;
    }
    return NULL;
}

unsigned int hashtable_count(hashtable *h) {
    return h->entrycount;
}

void hashtable_destroy(hashtable *h, int free_values) {
    unsigned int i;
    struct entry *e, *f;
    struct entry **table = h->table;
    if (free_values)
    {
        for (i = 0; i < h->length; i++)
        {
            e = table[i];
            while (NULL != e) { 
                f = e; 
                e = e->next; 
                free(f->key); 
                free(f->value); 
                free(f); 
            }
        }
    }
    else
    {
        for (i = 0; i < h->length; i++)
        {
            e = table[i];
            while (NULL != e) { 
                f = e; 
                e = e->next; 
                free(f->key);
                free(f); 
            }
        }
    }
    free(h->table);
    free(h);
}