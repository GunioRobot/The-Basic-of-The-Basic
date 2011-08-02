//
//  rbtree.h
//  red-black-tree
//
//  Created by Guanshan Liu on 02/08/2011.
//  Copyright 2011 Guanshan Liu. All rights reserved.
//
//  based on prasanthmadhavan's red-black-tree. A lot of 
//  changes are made.
//

#ifndef red_black_tree_rbtree_h
#define red_black_tree_rbtree_h

typedef enum {
    RED     = 0,
    BLACK   = 1
} rbtree_node_color;

typedef struct rbtree_node {
    void *value;
    struct rbtree_node *left;
    struct rbtree_node *right;
    struct rbtree_node *parent;
    rbtree_node_color color;
} rbtree_node;

typedef rbtree_node *rbtree_node_t;

typedef struct {
    rbtree_node *root;
} rbtree;

typedef rbtree *rbtree_t;

typedef int (*cmp_func)(void *left, void *right);

rbtree_t rbtree_create();
void *rbtree_search(rbtree_t t, void *value, cmp_func compare);
int rbtree_insert(rbtree_t t, void *value, cmp_func compare);
int rbtree_remove(rbtree_t t, void *value, cmp_func compare);

rbtree_node_t rbtree_grandparent(rbtree_node_t n);
rbtree_node_t rbtree_sibling(rbtree_node_t n);
rbtree_node_t rbtree_uncle(rbtree_node_t n);
int rbtree_verify_properties(rbtree_t t);
rbtree_node_color rbtree_get_node_color(rbtree_node_t n);
void rbtree_replace_node(rbtree_t t, rbtree_node_t oldn, rbtree_node_t newn);
rbtree_node_t rbtree_maximum_node(rbtree_node_t n);

#endif
