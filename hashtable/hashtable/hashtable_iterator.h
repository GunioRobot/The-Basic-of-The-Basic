//
//  hashtable_iterator.h
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

#ifndef hashtable_hashtable_iterator_h
#define hashtable_hashtable_iterator_h

#include "hashtable.h"

typedef struct{
    hashtable *h;
    entry *e;
    entry *parent;
    unsigned int index;
} hashtable_iterator;

hashtable_iterator *hashtable_iterator_create(hashtable *h);

extern inline void *hashtable_iterator_key(hashtable_iterator *i) {
    return i->e->key;
}

extern inline void *hashtable_iterator_value(hashtable_iterator *i) {
    return i->e->value;
}

int hashtable_iterator_advance(hashtable_iterator *itr);

int hashtable_iterator_remove(hashtable_iterator *itr);

int hashtable_iterator_search(hashtable_iterator *itr, hashtable *h, void *k);

#endif
