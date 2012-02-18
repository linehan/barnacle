// vim:fdm=marker
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

// binary search tree
struct bstree {
        uint32_t data;
        uint32_t key;
        struct bstree *left;
        struct bstree *right;
};

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
        node->data  = 0;
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
