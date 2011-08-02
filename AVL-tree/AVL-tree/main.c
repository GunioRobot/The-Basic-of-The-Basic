//
//  main.c
//  AVL-tree
//
//  Created by Guanshan Liu on 02/08/2011.
//  Copyright 2011 Guanshan Liu. All rights reserved.
//

#include<stdio.h>
#include<stdlib.h>

typedef struct avl_node {
    void *data;
    struct avl_node *parent;
    struct avl_node *left;
    struct avl_node *right;
} avl_node;

typedef avl_node *avl_node_t;

typedef struct {
    avl_node_t root;
} avl_tree;

typedef avl_tree *avl_tree_t;

typedef int (*cmp_func)(void *left, void *right);

avl_tree_t avl_tree_create();
avl_node_t avl_tree_node_create(void *value);
avl_node_t avl_tree_lookup(avl_tree_t t, void *value, cmp_func compare);
int avl_tree_height(avl_node_t cur);
int avl_tree_balance_factor(avl_node_t root);
void avl_tree_rotate_right(avl_tree_t t, avl_node_t main, avl_node_t pivot);
void avl_tree_rotate_left(avl_tree_t t, avl_node_t main, avl_node_t pivot);
void avl_tree_balance(avl_tree_t t, avl_node_t n);

avl_tree_t avl_tree_create() {
    avl_tree_t t = (avl_tree_t)malloc(sizeof(avl_tree));
    if (t == NULL) {
        return NULL;
    }
    t->root = NULL;
    return t;
}

avl_node_t avl_tree_node_create(void *value) {
    avl_node_t n = (avl_node_t)malloc(sizeof(avl_node));
    if (n == NULL) {
        return NULL;
    }
    n->data = value;
    n->left = NULL;
    n->right = NULL;
    n->parent = NULL; // not associated with parent here
    return n;
}

avl_node_t avl_tree_lookup(avl_tree_t t, void *value, cmp_func compare) {
    if (t == NULL || t->root == NULL) {
        return NULL;
    }
    avl_node_t n = t->root;
    while (n != NULL) {
        int result = compare(n->data, value);
        if (result == 0) {
            return n;
        }
        else if (result > 0) {
            n = n->right;
        }
        else {
            n = n->left;
        }
    }
    return NULL;
}

int avl_tree_height(avl_node_t cur) {
    if (cur == NULL) {
        return 0;
    }
    
    int l = 1 + avl_tree_height(cur->left);
    int r = 1 + avl_tree_height(cur->right);
    return l >= r ? l : r;
}

int avl_tree_balance_factor(avl_node_t root) {
    if (root == NULL) {
        return 0;
    }
    return avl_tree_height(root->left) - avl_tree_height(root->right);
}

void avl_tree_rotate_right(avl_tree_t t, avl_node_t main, avl_node_t pivot) {
    avl_node_t hold = NULL;
    pivot->parent = main->parent;
    if (main->parent != NULL && main == main->parent->left) {
        main->parent->left = pivot;
    }
    else if (main->parent != NULL && main == main->parent->right) {
        main->parent->right = pivot;
    }
    else {
        pivot->parent = NULL;
        t->root = pivot;
    }
    if (pivot->right != NULL) {
        hold = pivot->right;
    }
    pivot->right = main;
    pivot->right->left = hold;
    if (hold) {
		hold->parent = pivot->right;
    }
    main->parent = pivot;
}

void avl_tree_rotate_left(avl_tree_t t, avl_node_t main, avl_node_t pivot) {
    avl_node_t hold = NULL;
    pivot->parent = main->parent;
    if (main->parent != NULL && main == main->parent->right) {
        main->parent->right = pivot;
    }
    else if (main->parent != NULL && main == main->parent->left) {
        main->parent->left = pivot;
    }
    else {
        pivot->parent = NULL;
        t->root = pivot;
    }
    if (pivot->left != NULL) {
        hold = pivot->left;
    }
    pivot->left = main;
    pivot->left->right = hold;
    if (hold) {
		hold->parent = pivot->left;
    }
    main->parent = pivot;
}

void avl_tree_balance(avl_tree_t t, avl_node_t n) {
    if (avl_tree_balance_factor(n) == 2 && avl_tree_balance_factor(n->left) >= 0) {
        avl_tree_rotate_right(t, n, n->left);
    }
    else if (avl_tree_balance_factor(n) == -2 && avl_tree_balance_factor(n->right) <= 0) {
        avl_tree_rotate_left(t, n, n->right);
    }
    else if (avl_tree_balance_factor(n) == -2 && avl_tree_balance_factor(n->right) >= 0) {
        avl_tree_rotate_right(t, n->right, n->right->left);
        avl_tree_rotate_left(t, n, n->right);
    }
    else if (avl_tree_balance_factor(n) == 2 && avl_tree_balance_factor(n->right) <= 0) {
        avl_tree_rotate_left(t, n->left, n->left->right);
        avl_tree_rotate_right(t, n, n->left);
    }
}

void avl_tree_insert(avl_tree_t t, void *value, cmp_func compare);
void avl_insert(avl_tree_t t, avl_node_t n, void *value, cmp_func compare);
void avl_balance(avl_tree_t t, avl_node_t n);

void avl_insert(avl_tree_t t, avl_node_t n, void *value, cmp_func compare) {
    if (n == NULL && n == t->root) {
        t->root = avl_tree_node_create(value);
    }
    else {
        int result = compare(value, n->data);
        if (result == 0) {
            return;
        }
        else if (result > 0) {
            if (n->right != NULL) {
                return avl_insert(t, n->right, value, compare);
            }
            else {
                n->right = avl_tree_node_create(value);
                n->right->parent = n;
            }
        }
        else {
            if (n->left != NULL) {
                return avl_insert(t, n->left, value, compare);
            }
            else {
                n->left = avl_tree_node_create(value);
                n->left->parent = n;
            }
        }
    }
}

void avl_balance(avl_tree_t t, avl_node_t n) {
    if (n == NULL) {
        return;
    }
    while(n->parent != NULL) {
        avl_tree_balance(t, n->parent);
        if (n->parent) {
            n = n->parent;
        }
    }
}

void avl_tree_insert(avl_tree_t t, void *value, cmp_func compare) {
    if (t == NULL) {
        t = avl_tree_create();
        if (t == NULL) {
            return;
        }
    }
    avl_insert(t, t->root, value, compare);
    if(t->root != NULL) {
        avl_balance(t, avl_tree_lookup(t, value, compare));
    }
}

void avl_tree_parent_change(avl_tree_t t, avl_node_t min, avl_node_t p);
avl_node_t avl_tree_delete_replace(avl_node_t victim);
void avl_tree_delete(avl_tree_t t, void *value, cmp_func compare);
avl_node_t avl_delete(avl_tree_t t, void *value, cmp_func compare);

void avl_tree_parent_change(avl_tree_t t, avl_node_t min, avl_node_t p) {
    min->parent = p->parent;
    if(p->parent != NULL && p == p->parent->right) p->parent->right = min;
    else if(p->parent != NULL && p == p->parent->left) p->parent->left=min;
    else t->root = min;
}

avl_node_t avl_tree_delete_replace(avl_node_t victim) {
    avl_node_t t;
    if((!victim->right) && (!victim->left)) return victim;
    else if(victim->right)
    {
        if(!victim->right->left) return victim->right;
        else{
            for(t=victim->right;t->left!=0;t=t->left);
            return t;
        }
    }
    else if(victim->left)
    {
        if(!victim->left->right) return victim->left;
        else{
            for(t=victim->left;t->right!=0;t=t->right);
            return t;
        }
    }
    return NULL;
}

avl_node_t avl_delete(avl_tree_t t, void *value, cmp_func compare) {
    avl_node_t p = avl_tree_lookup(t, value, compare);
    if (p == NULL) {
        return NULL;
    }
    avl_node_t min = avl_tree_delete_replace(p);
    if(p->right != NULL){
        if(min!=p->right){
            min->parent->left = NULL;
            min->right = p->right;
            min->right->parent = min;
        }
        if(p->left != NULL){
            min->left = p->left;
            min->left->parent = min;
            avl_tree_parent_change(t, min, p);
        }
    }
    else if(min == p->left){
        min->right=0;
        avl_tree_parent_change(t, min, p);
    }
    else{
        if(p->parent){
			if(p==p->parent->right) p->parent->right=0;
			else p->parent->left=0;
        }
    }
    free(p);
    return min;
}

void avl_tree_delete(avl_tree_t t, void *value, cmp_func compare) {
    avl_node_t par = avl_delete(t, value, compare);
    if (par == NULL) {
        return;
    }
    if(par->left) avl_balance(t, par->left);
    else if(par->right) avl_balance(t, par->right);
    else avl_balance(t, par);
}


int main (int argc, const char * argv[])
{

    // insert test/implementation code here...
    printf("AVL tree...!\n");
    return 0;
}

