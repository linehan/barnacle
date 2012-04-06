// vim:fdm=marker
#ifndef __BINARY_SEARCH_TREES
#define __BINARY_SEARCH_TREES
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>


//struct qtree_node {
        //uint32_t key;
        //void *data;
        //struct qtree *NW;
        //struct qtree *NE;
        //struct qtree *SW;
        //struct qtree *SE;
//};


//static int qtree_insert(struct qtree_node *node, uint32_t key)
//{

struct bstree {
        uint32_t key;
        uint32_t data;
        struct bstree *left;
        struct bstree *right;
};

enum quadrants {QNW=0, QNE=1, QSW=2, QSE=3};


struct qtree {
        uint32_t key;
        void *data;
        struct qtree *quad[4];
};


static uint32_t msb(uint32_t a, uint32_t b)
{
       return (a ^ b);
}

static struct qtree *new_qtree(uint32_t key)
{
        struct qtree *new = malloc(sizeof(struct qtree));
        
        new->key  = key;
        new->data = NULL;
        int i;
        for (i=0; i<4; i++) {
                new->quad[i] = NULL;
        }
        return (new);
}

static struct qtree *qtree_split(struct qtree *q)
{
        q->quad[0] = qtree_insert(q, q->key);
}

static struct qtree *qtree_insert(struct qtree *q, uint32_t key)
{
        if (q == NULL) 
                return (new_qtree(key));
        else {
                uint32_t qindex = msb(key, q->key); // get quadrant number
                return (qtree_insert(q->quad[qindex], key));
        }
}

static void *qtree_get(struct qtree *node, uint32_t match)
{
        if (node == NULL)
                return (NULL);
        else {
                if (match == node->key)
                        return (node->data);
                else {
                        uint32_t qindex = msb(match, node->key);
                        return(qtree_get(node->quad[qindex], match));
                }
        }
}

        

static int bst_lookup(struct bstree *node, uint32_t match) 
{
        // 1. Base case == empty tree
        // in that case, the target is not found so return false
        if (node == NULL) 
                return(0);
        else {
                // 2. see if found here
                if (match == node->key) 
                        return(1);
                else {
                        // 3. otherwise recur down the correct subtree
                        if (match < node->key) 
                                return(bst_lookup(node->left, match));
                        else 
                                return(bst_lookup(node->right, match));
                }
        }
}

static uint32_t *bst_get(struct bstree* node, uint32_t match) {
        // 1. Base case == empty tree
        // in that case, the target is not found so return false
        if (node == NULL) 
                return(NULL);
        else {
                // 2. see if found here
                if (match == node->key) 
                        return(&(node->data));
                else {
                        // 3. otherwise recur down the correct subtree
                        if (match < node->key) 
                                return(bst_get(node->left, match));
                        else 
                                return(bst_get(node->right, match));
                }
        }
}

static struct bstree *new_bsnode(uint32_t key) {
        struct bstree* node = malloc(sizeof(struct bstree)); 
        node->key   = key;
        node->left  = NULL;
        node->right = NULL;
        return(node);
}
 
static struct bstree *bst_insert(struct bstree *node, uint32_t key) {
        // 1. If the tree is empty, return a new, single node
        if (node == NULL) {
                return(new_bsnode(key));
        }
        else {
                // 2. Otherwise, recur down the tree
                if (key <= node->key) 
                        node->left = bst_insert(node->left, key);
                else 
                        node->right = bst_insert(node->right, key);

                return(node); // return the (unchanged) node pointer
        }
} 

#endif
