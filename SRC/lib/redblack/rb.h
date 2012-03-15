#include <stdint.h>
#ifndef __RED_BLACK_TREES
#define __RED_BLACK_TREES

<<<<<<< .merge_file_sqexYq
#define DEFAULT_WORD_COUNT 1

struct rb_node {
        uint32_t key;
        uint32_t *data;
        void *extra;
=======
struct rb_node {
        uint32_t key;
        void *data;
>>>>>>> .merge_file_qnogmq
        int red;
        struct rb_node *link[2];
};

struct rb_tree {
        struct rb_node *root;
<<<<<<< .merge_file_sqexYq
        struct rb_node *peek;
        uint8_t words;
        int n;
};

struct rb_node *_rb_retreive(struct rb_node *node, uint32_t key);
/* Useful because all the other functions expect struct rb_tree pointers */
static inline struct rb_node *rb_retreive(struct rb_tree *tree, uint32_t key)
{
        return (_rb_retreive(tree->root, key));
}

void rb_store(struct rb_tree *tree, uint32_t key, void *extra);
struct rb_tree *new_tree(int words);
void rb_init(struct rb_tree *tree);
void rb_peek(struct rb_tree *tree, uint32_t key);
void *rb_extra(struct rb_tree *tree, uint32_t key);
int rb_remove(struct rb_tree *tree, uint32_t key);
int rb_insert(struct rb_tree *tree, uint32_t key);

=======
        int n;
};

int rb_remove(struct rb_tree *tree, uint32_t key);
int rb_insert(struct rb_tree *tree, uint32_t key);
void *rb_retreive(struct rb_tree *tree, uint32_t key);
>>>>>>> .merge_file_qnogmq

#endif
