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

/*
  The subexpression (v - 0x01010101UL), evaluates to a high bit set in any 
  byte whenever the corresponding byte in v is zero or greater than 0x80. 
  The sub-expression ~v & 0x80808080UL evaluates to high bits set in bytes 
  where the byte of v doesn't have its high bit set (so the byte was less 
  than 0x80). Finally, by ANDing these two sub-expressions the result is 
  the high bits set where the bytes in v were zero, since the high bits 
  set due to a value greater than 0x80 in the first sub-expression are 
  masked off by the second. 

  Juha Järvi suggested hasless(v, 1) on April 6, 2005, which he found on 
  Paul Hsieh's Assembly Lab; previously it was written in a newsgroup post 
  on April 27, 1987 by Alan Mycroft. 
*/
#define hasz(v) (((v) - 0x01010101UL) & ~(v) & 0x80808080UL)

/*
  We may want to know if any byte in a word has a specific value. To do 
  so, we can XOR the value to test with a word that has been filled with 
  the byte values in which we're interested. Because XORing a value with 
  itself results in a zero byte and nonzero otherwise, we can pass the 
  result to haszero. 

  Stephen M Bennet suggested this on December 13, 2009 after reading the 
  entry for haszero. 
*/
#define hasv(x,n) \
(hasz((x) ^ (~0UL/255 * (n))))
