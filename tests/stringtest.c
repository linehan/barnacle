#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int get_wind(const char *string)
{
  if ((strcmp(string, "direction")) == 0) return 1;   
  else if ((strcmp(string, "strength")) == 0) return 2;
  else if ((strcmp(string, "prevailing")) == 0) return 3;
  else perror ("INVALID ARGUMENT TO get_wind()!");
  return 0;
}
int32_t convertstring(const char *textbuff)
{
        int32_t mashed =
        textbuff[0] << 24 | 
        textbuff[1] << 16 |
        textbuff[2] << 8  |
        textbuff[3];
        return mashed;
}
enum { hoho = 1751410432 };
int main(void)
{
  int32_t a, b, c, d, e, f, g, h, i, j, k, l, m, n, x, bbb;

  /*a = get_wind("balloon");*/
  /*b = get_wind("direction");*/
  /*c = get_wind("strength");*/
  /*d = get_wind("prevailing");*/
  
  a = convertstring("hdg");
  b = convertstring("rig");
  x = convertstring("anc");
  bbb = convertstring("buf");
  c = convertstring("now");
  d = convertstring("req");
  e = convertstring("pan");
  f = convertstring("win");
  g = convertstring("dim");
  h = convertstring("h");
  i = convertstring("w");
  j = convertstring("y0");
  k = convertstring("x0");
  l = convertstring("dy");
  m = convertstring("dx");
  n = convertstring("n");



  /*printf("%d, %d, %d, %d", a, b, c, d);*/
  printf("hdg: %d\n", a);
  printf("rig: %d\n", b);
  printf("anc: %d\n", x);
  printf("buf: %d\n", bbb);
  printf("now: %d\n", c);
  printf("req: %d\n", d);
  printf("pan: %d\n", e);
  printf("win: %d\n", f);
  printf("dim: %d\n", g);
  printf("  h: %d\n", h);
  printf("  w: %d\n", i);
  printf(" y0: %d\n", j);
  printf(" x0: %d\n", k);
  printf(" dy: %d\n", l);
  printf(" dx: %d\n", m);
  printf("  n: %d\n", n);

  if (a == hoho) {
          printf("enums work for this\n");
          printf("hdg: %d\nenum: %d", a, hoho);
}
  
  return 0;
}

