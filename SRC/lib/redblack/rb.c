#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "rb.h"
/* 
  RED BLACK TREES 

  1. A node is either red or black.
  2. The root is black. (sometimes)
  3. All leaves are the same color as the root.
  4. Both children of every red node are black.
  5. Every simple path from a given node to any of its descendant 
     leaves contains the same number of black nodes.
*/


struct rb_node *make_node(uint32_t key)
{
        struct rb_node *new = malloc(sizeof *new);

        if (new != NULL) {
                new->key = key;
                new->red = 1; // 1 is red, 0 is black
                new->link[0] = NULL;
                new->link[1] = NULL;
                new->data = 0;
                new->box = NULL;
        }
        return (new);
}

struct rb_tree *new_tree(void)
{
        struct rb_tree *new = malloc(sizeof *new);
        new->root = NULL;
        new->peek = NULL;
        new->n = 0;
        return (new);
}


void rb_init(struct rb_tree *tree)
{
        tree->root = NULL;
        tree->peek = NULL;
        tree->n = 0;
}

int is_red(struct rb_node *root)
{
        return root != NULL && root->red == 1;
}


/*
  Single rotation roaates the nodes as normal, then sets the old root to be
  red and the new root to be black.
*/
struct rb_node *rb_rot_single(struct rb_node *root, int dir)
{
        struct rb_node *save = root->link[!dir];

        root->link[!dir] = save->link[dir];
        save->link[dir]  = root;

        root->red = 1;
        save->red = 0;

        return (save);
}


/*
  Double rotation is just two single rotations.
*/
struct rb_node *rb_rot_double(struct rb_node *root, int dir)
{
        root->link[!dir] = rb_rot_single(root->link[!dir], !dir);
        return (rb_rot_single(root, dir));
}


/*
  Test for various erroneous conditions.
*/
int rb_assert(struct rb_node *root)
{
        int lh, rh;
        if (root == NULL)
                return 1;
        else {
                struct rb_node *ln = root->link[0];
                struct rb_node *rn = root->link[1];

                // Consecutive red links
                if (is_red(root)) {
                        if(is_red(ln) || is_red(rn)) {
                                puts("Red violation");
                                return (0);
                        }
                }
                lh = rb_assert(ln);
                rh = rb_assert(ln);
                // Invalid binary search tree
                if (  (ln != NULL && ln->key >= root->key)
                    ||(rn != NULL && rn->key <= root->key))
                {
                        puts("Binary tree violation");
                        return (0);
                }
                // Black height mismatch
                if (lh != 0 && rh != 0 && lh != rh) {
                        puts("Black violation");
                        return (0);
                }
                // Only count black links
                if (ln != 0 && rh != 0)
                        return is_red(root) ? lh : lh+1;
                else
                        return (0);
        }
}


/*
  The recursive part of the recursive insertion routine.
*/
struct rb_node *rb_insert_r(struct rb_node *root, uint32_t key)
{
        if (root == NULL)
                root = make_node(key);
        else if (key != root->key) {
                int dir = root->key < key;

                root->link[dir] = rb_insert_r(root->link[dir], key);

                // Rebalancing
                if (is_red(root->link[dir])) {
                        if (is_red(root->link[!dir])) {
                                // Case 1
                                root->red = 1;
                                root->link[0]->red = 0;
                                root->link[1]->red = 0;
                        }
                        else {
                                // Cases 2&3
                                if (is_red(root->link[dir]->link[dir]))
                                        root = rb_rot_single(root, !dir);
                                else if (is_red(root->link[dir]->link[dir]))
                                        root = rb_rot_double(root, !dir);
                        }
                }
        }
        return (root);
}


/*
  The recursive insertion routine.
*/
int rb_insert_recurse(struct rb_tree *tree, uint32_t key)
{
        tree->root = rb_insert_r(tree->root, key);
        tree->root->red = 0;
        return (1);
}


/*
   The top-down single-pass insertion routine.
*/
int rb_insert(struct rb_tree *tree, uint32_t key)
{
        if (tree->root == NULL) {
                tree->root = make_node(key);
                if (tree->root == NULL) // malloc screwed us
                        return (0);
        }
        else {
                struct rb_node head = {0}; // false root
                struct rb_node *g, *t;     // grandparent and parent
                struct rb_node *p, *q;     // iterator and parent
                int dir = 0, last;

                // Helpers
                t = &head;
                g = p = NULL;
                q = t->link[1] = tree->root;

                // Search down the tree
                for (;;) { 
                        if (q == NULL) {
                                // Insert new node at the bottom
                                p->link[dir] = q = make_node(key);
                                if (q == NULL)
                                        return (0);
                        }
                        else if (is_red(q->link[0]) && is_red(q->link[1])) {
                                // Color flip
                                q->red = 1;
                                q->link[0]->red = 0;
                                q->link[1]->red = 0;
                        }
                        // Fix red violation
                        if (is_red(q) && is_red(p)) {
                                int dir2 = t->link[1] == g;

                                if (q == p->link[last])
                                        t->link[dir2] = rb_rot_single(g, !last);
                                else
                                        t->link[dir2] = rb_rot_double(g, !last);
                        }
                        // Stop if found
                        if (q->key == key)
                                break; // out of the infinite for loop

                        last = dir;
                        dir  = q->key < key;

                        // Update helpers
                        if (g != NULL)
                                t = g;
                        g = p, p = q;
                        q = q->link[dir];
                }
                tree->root = head.link[1]; // Update root
        }
        tree->root->red = 0; // Make root black

        return (1);
}


/*
  The dreaded top-down single pass deletion
*/
int rb_remove(struct rb_tree *tree, uint32_t key)
{
        if (tree->root != NULL) {
                struct rb_node head = {0}; // False root
                struct rb_node *q, *p, *g; // Helpers (see rb_insert())
                struct rb_node *f = NULL;  // Found item
                int dir = 1;

                // Set up helpers
                q = &head;
                g = p = NULL;
                q->link[1] = tree->root;

                // Search and push a red down
                while (q->link[dir] != NULL) {
                        int last = dir;

                        // Update helpers
                        g = p, p = q;
                        q = q->link[dir];
                        dir = q->key < key;

                        // Save found node
                        if (q->key == key)
                                f = q;

                        // Push the red node down
                        if (!is_red(q) && !is_red(q->link[dir])) {
                                if (is_red(q->link[!dir]))
                                        p = p->link[last] = rb_rot_single(q, dir);
                                else if (!is_red(q->link[!dir])) {
                                        struct rb_node *s = p->link[!last];

                                        if (s != NULL) {
                                                if (!is_red(s->link[!last]) && !is_red(s->link[last])) {
                                                        // Color flip
                                                        p->red = 0;
                                                        s->red = 1;
                                                        q->red = 1;
                                                }
                                                else {
                                                        int dir2 = g->link[1] == p;

                                                        if (is_red(s->link[last]))
                                                                g->link[dir2] = rb_rot_double(p, last);
                                                        else if (is_red(s->link[!last]))
                                                                g->link[dir2] = rb_rot_single(p, last);

                                                        // Ensure correct coloring
                                                        q->red = g->link[dir2]->red = 1;
                                                        g->link[dir2]->link[0]->red = 0;
                                                        g->link[dir2]->link[1]->red = 0;
                                                }
                                        }
                                }
                        }
                }
                // Replace and remove if found
                if (f != NULL) {
                        f->key = q->key;
                        p->link[p->link[1] == q] = q->link[q->link[0] == NULL];
                        free (q);
                }
                // Update root and make it black
                tree->root = head.link[1];
                if (tree->root != NULL)
                        tree->root->red = 0;
        }
        return (1);
}

struct rb_node *rb_retreive(struct rb_node *node, uint32_t key)
{
        if (node == NULL) 
                return (NULL);

        if (key == node->key) 
                return (node);
        else {
                if (key < node->key)
                        rb_retreive(node->link[0], key); // go left
                else if (key > node->key)
                        rb_retreive(node->link[1], key); // go right
        }
        return (NULL); // Nothing found
}

void rb_peek(struct rb_tree *tree, uint32_t key)
{
        if (tree->root == NULL)
                return;
        else 
                tree->peek = rb_retreive(tree->root, key);
}

void rb_store(struct rb_tree *tree, uint32_t key, void *extra)
{
        rb_peek(tree, key);
        if (tree->peek == NULL) { // No entry with this key yet exists.
                rb_insert(tree, key); // Insert a new record.
                rb_store(tree, key, extra); // Call self recursively.
        }
        else
                tree->peek->box = extra; // Attach extra to node.
}
