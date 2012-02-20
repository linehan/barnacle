// Bit twiddling macros starting with '_' work on the entire word
#define _SET(B)       B =~ 0 
#define _IS_SET(B)    B &  1
#define _TOGGLE(B)    B ^= 1
#define _CLEAR(B)     B &  0

// These macros operate on the n-th bit
#define SET(B, n)     B |=  (1 << n) 
#define IS_SET(B, n)  B &   (1 << n)
#define TOGGLE(B, n)  B ^=  (1 << n)
#define CLEAR(B, n)   B &= ~(1 << n)
