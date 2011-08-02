//
//  rbtree.c
//  red-black-tree
//
//  Created by Guanshan Liu on 02/08/2011.
//  Copyright 2011 Guanshan Liu. All rights reserved.
//
//  based on prasanthmadhavan's red-black-tree. A lot of 
//  changes are made.
//

#include <stdio.h>
#include <stdlib.h>
#include "rbtree.h"

rbtree_t rbtree_create() {
    rbtree_t t = (rbtree_t)malloc(sizeof(rbtree));
    if (t == NULL) {
        return NULL;
    }
    t->root = NULL;
    return t;
}

rbtree_node_t rbtree_grandparent(rbtree_node_t n) {
    if (n == NULL || n->parent == NULL) {
        return NULL;
    }
    return n->parent->parent;
}

rbtree_node_t rbtree_sibling(rbtree_node_t n) {
    if (n == NULL || n->parent == NULL) {
        return NULL;
    }
    if (n == n->parent->left) {
        return n->parent->right;
    }
    else {
        return n->parent->left;
    }
}

rbtree_node_t rbtree_uncle(rbtree_node_t n) {
    if (n == NULL || n->parent == NULL) {
        return NULL;
    }
    return rbtree_sibling(n->parent);
}

rbtree_node_color rbtree_get_node_color(rbtree_node_t n) {
    return n == NULL ? BLACK : n->color;
}

int property_check_1(rbtree_node_t n);
int property_check_1(rbtree_node_t n) {
    if (n == NULL) {
        return -1;
    }
    if (n->color != RED && n->color != BLACK) {
        return 0;
    }
    if (property_check_1(n->left) == 0) {
        return 0;
    }
    if (property_check_1(n->right) == 0) {
        return 0;
    }
    return -1;
}

int property_check_4(rbtree_node_t n);
int property_check_4(rbtree_node_t n) {
    if (rbtree_get_node_color(n) == RED) {
        if (rbtree_get_node_color(n->left) == BLACK ||
            rbtree_get_node_color(n->right) == BLACK ||
            rbtree_get_node_color(n->parent) == BLACK) {
            return 0;
        }
    }
    if (n == NULL) {
        return -1;
    }
    if (property_check_4(n->left) == 0) {
        return 0;
    }
    if (property_check_4(n->right) == 0) {
        return 0;
    }
    return -1;
}

int property_check_5_helper(rbtree_node_t n, int black_count, int *black_count_path);
int property_check_5_helper(rbtree_node_t n, int black_count, int *black_count_path) {
    if (rbtree_get_node_color(n) == BLACK) {
        black_count++;
    }
    if (n == NULL) {
        if (*black_count_path == -1) {
            *black_count_path = black_count;
        }
        else if (black_count != *black_count_path) {
            return 0;
        }
        return -1;
    }
    if (property_check_5_helper(n->left, black_count, black_count_path) == 0) {
        return 0;
    }
    if (property_check_5_helper(n->right, black_count, black_count_path) == 0) {
        return 0;
    }
    return -1;
}

int property_check_5(rbtree_node_t n);
int property_check_5(rbtree_node_t n) {
    int bcp = -1;
    return property_check_5_helper(n, 0, &bcp);
}

int rbtree_verify_properties(rbtree_t t) {
    if (property_check_1(t->root) == 0) {
        return 0;
    }
    if (rbtree_get_node_color(t->root) != BLACK) {
        return 0;
    }
    if (property_check_4(t->root) == 0) {
        return 0;
    }
    if (property_check_5(t->root) == 0) {
        return 0;
    }
    return -1;
}

rbtree_node_t new_node(void *value, rbtree_node_color color, rbtree_node_t left, rbtree_node_t right);
rbtree_node_t new_node(void *value, rbtree_node_color color, rbtree_node_t left, rbtree_node_t right) {
    rbtree_node_t n = (rbtree_node_t)malloc(sizeof(rbtree_node));
    if (n == NULL) {
        return NULL;
    }
    n->value = value;
    n->color = color;
    n->left = left;
    n->right = right;
    if (left != NULL) {
        left->parent = n;
    }
    if (right != NULL) {
        right->parent = n;
    }
    n->parent = NULL;
    return n;
}

rbtree_node_t lookup_node(rbtree_t t, void *value, cmp_func compare);
rbtree_node_t lookup_node(rbtree_t t, void *value, cmp_func compare) {
    rbtree_node_t n = t->root;
    while (n != NULL) {
        int result = compare(value, n->value);
        if (result == 0) {
            return n;
        }
        else if (result < 0) {
            n = n->left;
        }
        else {
            n = n->right;
        }
    }
    return NULL;
}

void *rbtree_search(rbtree_t t, void *value, cmp_func compare) {
    rbtree_node_t n = lookup_node(t, value, compare);
    return n == NULL ? NULL : n->value;
}

rbtree_node_t rbtree_maximum_node(rbtree_node_t n) {
    while (n != NULL) {
        n = n->right;
    }
    return n;
}

void rbtree_replace_node(rbtree_t t, rbtree_node_t oldn, rbtree_node_t newn) {
    if (oldn == NULL) {
        return;
    }
    if (oldn->parent == NULL) {
        t->root = newn;
    }
    else {
        if (oldn == oldn->parent->left) {
            oldn->parent->left = newn;
        }
        else {
            oldn->parent->right = newn;
        }
    }
    if (newn != NULL) {
        newn->parent = oldn->parent;
    }
}

void rotate_left(rbtree_t t, rbtree_node_t n);
void rotate_left(rbtree_t t, rbtree_node_t n) {
    rbtree_node_t r = n->right;
    rbtree_replace_node(t, n, r);
    n->right = r->left;
    if (r->left != NULL) {
        r->left->parent = n;
    }
    r->left = n;
    n->parent = r;
}

void rotate_right(rbtree_t t, rbtree_node_t n);
void rotate_right(rbtree_t t, rbtree_node_t n) {
    rbtree_node_t l = n->left;
    rbtree_replace_node(t, n, l);
    n->left = l->right;
    if (l->right != NULL) {
        l->right->parent = n;
    }
    l->right = n;
    n->parent = l;
}

void insert_case1(rbtree_t t, rbtree_node_t n);
void insert_case2(rbtree_t t, rbtree_node_t n);
void insert_case3(rbtree_t t, rbtree_node_t n);
void insert_case4(rbtree_t t, rbtree_node_t n);
void insert_case5(rbtree_t t, rbtree_node_t n);

void insert_case5(rbtree_t t, rbtree_node_t n) {
    n->parent->color = BLACK;
    rbtree_grandparent(n)->color = RED;
    if (n == n->parent->left && n->parent == rbtree_grandparent(n)->left) {
        rotate_right(t, rbtree_grandparent(n));
    } else if(n == n->parent->right && n->parent == rbtree_grandparent(n)->right) {
        rotate_left(t, rbtree_grandparent(n));
    }
}

void insert_case4(rbtree_t t, rbtree_node_t n) {
    if (n == n->parent->right && n->parent == rbtree_grandparent(n)->left) {
        rotate_left(t, n->parent);
        n = n->left;
    } else if (n == n->parent->left && n->parent == rbtree_grandparent(n)->right) {
        rotate_right(t, n->parent);
        n = n->right;
    }
    insert_case5(t, n);
}

void insert_case3(rbtree_t t, rbtree_node_t n) {
    if (rbtree_get_node_color(rbtree_uncle(n)) == RED) {
        n->parent->color = BLACK;
        rbtree_uncle(n)->color = BLACK;
        rbtree_grandparent(n)->color = RED;
        insert_case1(t, n);
    }
    else {
        insert_case4(t, n);
    }
}

void insert_case2(rbtree_t t, rbtree_node_t n) {
    if (rbtree_get_node_color(n->parent) == BLACK) {
        return;
    }
    insert_case3(t, n);
}

void insert_case1(rbtree_t t, rbtree_node_t n) {
    if (n->parent == NULL) {
        n->color = BLACK;
    }
    else {
        insert_case2(t, n);
    }
}

int rbtree_insert(rbtree_t t, void *value, cmp_func compare) {
    rbtree_node_t node = new_node(value, RED, NULL, NULL);
    if (node == NULL) {
        return 0;
    }
    if (t == NULL) {
        t = rbtree_create();
    }
    if (t == NULL) {
        return 0;
    }
    if (t->root == NULL) {
        t->root = node;
    }
    else {
        rbtree_node_t n = t->root;
        while (1) {
            int result = compare(value, n->value);
            if (result == 0) {
                free(node);
                return -1;
            }
            else if (result < 0) {
                if (n->left == NULL) {
                    n->left = node;
                    break;
                }
                else {
                    n = n->left;
                }
            }
            else {
                if (n->right == NULL) {
                    n->right = node;
                    break;
                }
                else {
                    n = n->right;
                }
            }
        }
        node->parent = n;
    }
    insert_case1(t, node);
    return rbtree_verify_properties(t);
}

void delete_case1(rbtree_t t, rbtree_node_t n);
void delete_case2(rbtree_t t, rbtree_node_t n);
void delete_case3(rbtree_t t, rbtree_node_t n);
void delete_case4(rbtree_t t, rbtree_node_t n);
void delete_case5(rbtree_t t, rbtree_node_t n);
void delete_case6(rbtree_t t, rbtree_node_t n);

void delete_case1(rbtree_t t, rbtree_node_t n) {
    if (n->parent == NULL)
        return;
    else
        delete_case2(t, n);
}

void delete_case2(rbtree_t t, rbtree_node_t n) {
    if (rbtree_get_node_color(rbtree_sibling(n)) == RED) {
        n->parent->color = RED;
        rbtree_sibling(n)->color = BLACK;
        if (n == n->parent->left)
            rotate_left(t, n->parent);
        else
            rotate_right(t, n->parent);
    }
    delete_case3(t, n);
}

void delete_case3(rbtree_t t, rbtree_node_t n) {
    if (rbtree_get_node_color(n->parent) == BLACK &&
        rbtree_get_node_color(rbtree_sibling(n)) == BLACK &&
        rbtree_get_node_color(rbtree_sibling(n)->left) == BLACK &&
        rbtree_get_node_color(rbtree_sibling(n)->right) == BLACK)
    {
        rbtree_sibling(n)->color = RED;
        delete_case1(t, n->parent);
    }
    else
        delete_case4(t, n);
}

void delete_case4(rbtree_t t, rbtree_node_t n) {
    if (rbtree_get_node_color(n->parent) == RED &&
        rbtree_get_node_color(rbtree_sibling(n)) == BLACK &&
        rbtree_get_node_color(rbtree_sibling(n)->left) == BLACK &&
        rbtree_get_node_color(rbtree_sibling(n)->right) == BLACK)
    {
        rbtree_sibling(n)->color = RED;
        n->parent->color = BLACK;
    }
    else
        delete_case5(t, n);
}

void delete_case5(rbtree_t t, rbtree_node_t n) {
    if (n == n->parent->left &&
        rbtree_get_node_color(rbtree_sibling(n)) == BLACK &&
        rbtree_get_node_color(rbtree_sibling(n)->left) == RED &&
        rbtree_get_node_color(rbtree_sibling(n)->right) == BLACK)
    {
        rbtree_sibling(n)->color = RED;
        rbtree_sibling(n)->left->color = BLACK;
        rotate_right(t, rbtree_sibling(n));
    }
    else if (n == n->parent->right &&
             rbtree_get_node_color(rbtree_sibling(n)) == BLACK &&
             rbtree_get_node_color(rbtree_sibling(n)->right) == RED &&
             rbtree_get_node_color(rbtree_sibling(n)->left) == BLACK)
    {
        rbtree_sibling(n)->color = RED;
        rbtree_sibling(n)->right->color = BLACK;
        rotate_left(t, rbtree_sibling(n));
    }
    delete_case6(t, n);
}

void delete_case6(rbtree_t t, rbtree_node_t n) {
    rbtree_sibling(n)->color = rbtree_get_node_color(n->parent);
    n->parent->color = BLACK;
    if (n == n->parent->left) {
        if (rbtree_get_node_color(rbtree_sibling(n)->right) == RED) {
            rbtree_sibling(n)->right->color = BLACK;
            rotate_left(t, n->parent);
        }
    }
    else
    {
        if (rbtree_get_node_color(rbtree_sibling(n)->left) == RED) {
            rbtree_sibling(n)->left->color = BLACK;
            rotate_right(t, n->parent);
        }
    }
}

int rbtree_remove(rbtree_t t, void *value, cmp_func compare) {
    rbtree_node_t child;
    rbtree_node_t n = lookup_node(t, value, compare);
    if (n == NULL) {
        return -1;
    }
    if (n->left != NULL && n->right != NULL) {
        rbtree_node_t pred = rbtree_maximum_node(n->left);
        n->value = pred->value;
        n = pred;
    }
    child = n->right == NULL? n->left : n->right;
    if (rbtree_get_node_color(n) == BLACK) {
        n->color = rbtree_get_node_color(child);
        delete_case1(t, n);
    }
    rbtree_replace_node(t, n, child);
    if (n->parent == NULL && child != NULL) {
        child->color = BLACK;
    }
    free(n);
    return rbtree_verify_properties(t);
}