#include <stdint.h>
#ifndef __RED_BLACK_TREES
#define __RED_BLACK_TREES

struct rb_node {
        uint32_t key;
        uint32_t data;
        void *box;
        int red;
        struct rb_node *link[2];
};

struct rb_tree {
        struct rb_node *root;
        struct rb_node *peek;
        int n;
};

int rb_remove(struct rb_tree *tree, uint32_t key);
int rb_insert(struct rb_tree *tree, uint32_t key);
struct rb_node *rb_retreive(struct rb_node *node, uint32_t key);
void rb_store(struct rb_tree *tree, uint32_t key, void *extra);
struct rb_tree *new_tree(void);
void rb_init(struct rb_tree *tree);

#endif
