// vim:fdm=marker
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

static enum {QNE = 0, QNW = 1, QSE = 2, QSW = 4};

struct quadtree {struct quadnode *Q; uint32_t size;};

struct quadnode {
        struct quadnode *go[4];
        uint32_t key;
        uint32_t val;
};

#define NEW_QNODE calloc(1, sizeof(struct quadnode));
#define NEW_QTREE calloc(1, sizeof(struct quadtree));


static inline void insert(node ** tree, node * item) {
   if(!(*tree)) {
      *tree = item;
      return;
   }
   if(item->val<(*tree)->val)
      insert(&(*tree)->left, item);
   else if(item->val>(*tree)->val)
      insert(&(*tree)->right, item);
}


/*

Each square in the quadtree has a side length which is a power of two, and 
corner coordinates which are multiples of the side length. 

Given any two points, the derived square for the two points is the smallest 
square covering both points. 

The interleaving of bits from the x and y components of each point is called 
the shuffle of x and y, and can be extended to higher dimensions.


The sides of all squares in the quadtree have lengths of the form 2^-i

For any square of side length 2^-i, the coordinates of all four corners
are integral multiples of 2^-i.

We define a square with bottom left corner (x,y) and size 2^-i to contain
a point (x',y') if x<=x' < x+2^-i and y<=y'<y+2^-i.

Given two input points (x,y) and (x',y'), we define their derived square
to be the smallest such square containing both points. The size of this 
square can be found by comparing the high order bits of x XOR x' and 
y XOR y', and the coordinates of its corners can be found by masking off
lower order bits.

Given a point (x,y), where x and y are 16-bit ints, we define the shuffle
Sh(x,y) to be the 32-bit int formed by alternately taking the bits of x
and y from most significant to least significant, the x bit before the y
bit (interleaving): xyxyxyxyxyxyxyxyxyxyxyxyxyxyxyxy. We represent Sh(x,y)
implicitly by the pair (x,y).

We can compare two numbers Sh(x,y) and Sh(x',y') in constant time, using
arithmetic and high bit operations separately on x and y.




        ABCDEFGHIJKL

        ds[0] = high order bits of Ax ^ Bx = size x
                high order bits of Ay ^ By = size y

        ds[1] = high order bits of Cx ^ Dx = size x
                high order bits of Cy ^ Dy = size y

        ...

        For each i in ds, the interval containing ds[i]
        is bounded by the first larger square to the right
        (ds[i++]) and to the left (ds[i--]).

        Each of these intervals corresponds to a square in the
        quadtree. This means that 





Once the points are in sorted order, two properties make it easy to build 
a quadtree: The first is that the points contained in a square of the 
quadtree form a contiguous interval in the sorted order. 

The second is that if more than one child of a square contains an input 
point, the square is the derived square for two adjacent points in the 
sorted order.

For each adjacent pair of points, the derived square is computed and its 
side length determined. For each derived square, the interval containing 
it is bounded by the first larger square to the right and to the left in 
sorted order.

Each such interval corresponds to a square in the quadtree. The result of 
this is a compressed quadtree, where only nodes containing input points or 
two or more children are present. A non-compressed quadtree can be built by 
restoring the missing nodes, if desired.

Rather than building a pointer based quadtree, the points can be maintained 
in sorted order in a data structure such as a binary search tree. This allows 
points to be added and deleted in O(log n) time. 

Two quadtrees can be merged by merging the two sorted sets of points, and 
removing duplicates. 

Point location can be done by searching for the points preceding and following 
the query point in the sorted order. If the quadtree is compressed, the 
predecessor node found may be an arbitrary leaf inside the compressed node of 
interest. In this case, it is necessary to find the predecessor of the least 
common ancestor of the query point and the leaf found.
*/
/*
   Turn coordinate pairs (x,y) into morton numbers and sort them.
   Mergesort works nicely. This is our sorted set {S}.

   We want an unbalanced quadtree

   If we remove all leaves of this tree that do not contain input
   points, and contract all remaining paths of nodes having only
   one child each, we get a tree {Tf} in which all internal nodes
   have degree 2 or more. We call {Tf} the -framework-, and we use
   it to construct the final quadtree.

   Useful facts about or framework {Tf}:
        - Its nodes correspond to the derived squares for adjacent points
          in the sorted set.

        - Its structure corresponds to the nesting of intervals induced
          by the derived squares.

   So to construct framework {Tf}, we follow this algorithm:
        1. For each adjacent pair of points in sorted set {S}, compute
           the derived square. 
           
        2. Record the side length of the derived square.

        3. Find the first larger derived square to the right and left
           in the sorted set {S}. [All-nearest-larger-values; O(log n)]
           This determines the nesting.

   Now that we have constructed framework {Tf}, we can construct the
   quadtree {Tq}. Useful facts about the relationship between {Tf}
   and {Tq}: 

        - Each edge in {Tf} corresponds to a path of perhaps
          many squares in {Tq}. The number of squares is
          determined by the relative sizes of {Tf} squares.


   






































