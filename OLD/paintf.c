/* This is an extremly simple (sn|as)printf like function
 * but even this is enough for a lot of people. However it's mainly here as
 * a "howto" for anyone who wants to know how these functions are created.
 * The most obvious missing bits are the floating point types
 * (%a, %A, %e, %E, %f, %F, %g, %G).
 *
 * Other bits missing are all the advanced features like i18n, thousands
 * specifiers, and various corner cases of the std. The reason they are missing
 * is to make it easier to understand (if you need one with those features see:
 * http://www.and.org/vstr/printf_comparison.html
 *
 * Also note that although the normal *printf functions return -1 on error,
 * thesse don't. the asprintf() also always returns *ptr a NULL, which glibc
 * doesn't.
 */

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <limits.h>

/* prototypes for the entry points to this file ... */
extern unsigned int simple_vsnprintf(char *, unsigned int, const char*, va_list)
   __attribute__ ((__format__ (__printf__, 3, 0)));
extern unsigned int simple_snprintf(char *, unsigned int, const char *, ...)
   __attribute__ ((__format__ (__printf__, 3, 4)));
extern unsigned int simple_asprintf(char **, const char *, ...)
   __attribute__ ((__format__ (__printf__, 2, 3)));
extern unsigned int simple_append_asprintf(char **, unsigned int *,
                                           const char *, ...)
   __attribute__ ((__format__ (__printf__, 3, 4)));

/* add data ... */
#define ADD_C(x) do { char addc = (x);                                  \
                      if (++ret >= sz) {} else *buf++ = addc; } while (0)

#define ADD_PAD_C(x, y) do { while (y) { ADD_C(x); --(y); } } while (0)

#define ADD_NUM(num, base, chrs) do {                                   \
      char num_buf[sizeof(uintmax_t) * CHAR_BIT];                       \
      char *ptr = num_buf;                                              \
                                                                        \
      ADD_PAD_C('0', zeros);                                            \
      num_ret = ret;                                                    \
                                                                        \
      do                                                                \
      {                                                                 \
        unsigned int chr_offset = (num % base);                         \
                                                                        \
        num /= base;                                                    \
                                                                        \
        *ptr++ = chrs[chr_offset];                                      \
      } while (num);                                                    \
                                                                        \
      while (ptr != num_buf) ADD_C(*--ptr);                             \
                                                                        \
    } while (0)

/* parse data ... */
#define PARSE_FMT_NUM10(x)                                              \
    while ((*fmt >= '0') && (*fmt <= '9')) (x) = (x) * 10 + (*fmt++ - '0')

static int match_cmd(const char **passed_fmt, const char *cmd)
{
  const char *fmt = *passed_fmt;

  while (*cmd && (*fmt++ == *cmd))
    ++cmd;

  if (!*cmd)
    *passed_fmt = fmt;

  return (!*cmd);
}

/* limited decrement */
#define DEC__VAL(x, y) do {                    \
      if ((unsigned int)(y) > (unsigned int)(x)) \
        (y) -= (x);                              \
      else                                       \
        (y)  = 0;                                \
    } while (0)
#define DEC_WIDTH(x) DEC__VAL(x, wid)
#define DEC_PREC(x)  DEC__VAL(x, prec)

/* all the helper macro function to format signed and unsigned numbers ... */
#define FMT_NUM_BEG()                                                   \
    unsigned int zeros   = 0;                                           \
    unsigned int espcs   = 0;                                           \
    unsigned int num_ret = ret;                                         \
                                                                        \
    if (prec < 0) prec = 0; else flg_zero = 0;                          \
                                                                        \
    if (wid || prec) {                                                  \
    char *       beg_buf  = buf;                                        \
    unsigned int beg_ret  = ret;                                        \
    unsigned int bspcs = 0
#define FMT_NUM_MID()                                                   \
    DEC_PREC(ret - num_ret);                                            \
    DEC_WIDTH((ret - beg_ret) + prec);                                  \
                                                                        \
    zeros = prec;                                                       \
                                                                        \
    if (0) {}                                                           \
    else if (flg_minus) espcs = wid;                                    \
    else if (flg_zero)  zeros = wid;                                    \
    else                bspcs = wid;                                    \
                                                                        \
    buf  = beg_buf;                                                     \
    ret  = beg_ret;                                                     \
                                                                        \
    ADD_PAD_C(' ', bspcs);                                              \
    }                                                                   \
    arg = tmp_arg
#define FMT_NUM_END()                                                   \
    ADD_PAD_C(' ', espcs)

#define FMT_NUM__S(tmp_arg, arg, base, chrs)                            \
      if (tmp_arg < 0) { ADD_C('-'); arg = -arg; }                      \
      else if (flg_plus) ADD_C('+');                                    \
      else if (flg_spac) ADD_C(' ');                                    \
                                                                        \
    ADD_NUM(arg, base, chrs)
#define FMT_NUM_S(cmd, dsT, pT, duT, base, chrs)                        \
    else if (match_cmd(&fmt, cmd)) do {                                 \
      dsT tmp_arg = va_arg(ap, pT);                                     \
      duT arg = tmp_arg;                                                \
                                                                        \
      FMT_NUM_BEG();                                                    \
      FMT_NUM__S(tmp_arg, arg, base, chrs);                             \
      FMT_NUM_MID();                                                    \
      FMT_NUM__S(tmp_arg, arg, base, chrs);                             \
      FMT_NUM_END();                                                    \
    } while (0)

#define FMT_NUM__U(arg, base, chrs, h1, h2)                             \
      if (flg_hash && h1) ADD_C(h1);                                    \
      if (flg_hash && h2) ADD_C(h2);                                    \
                                                                        \
      ADD_NUM(arg, base, chrs)
#define FMT_NUM_U(cmd, duT, pT, base, chrs, h1, h2)                     \
    else if (match_cmd(&fmt, cmd)) do {                                 \
      duT tmp_arg = va_arg(ap, pT);                                     \
      duT arg = tmp_arg;                                                \
                                                                        \
      FMT_NUM_BEG();                                                    \
      FMT_NUM__U(arg, base, chrs, h1, h2);                              \
      FMT_NUM_MID();                                                    \
      FMT_NUM__U(arg, base, chrs, h1, h2);                              \
      FMT_NUM_END();                                                    \
    } while (0)

/* main functions for numbers, these do all the different sizes for each number
 * Ie. %hhd, %hd, %d, %ld, %lld, %zd, %td, and %jd */
#define FMT_ALL_NUM_S(cmd, b, c)                                           \
    FMT_NUM_S("hh" cmd, signed char,       int,      unsigned char, b, c); \
    FMT_NUM_S("h"  cmd,       short,       int,     unsigned short, b, c); \
    FMT_NUM_S(     cmd,         int,       int,       unsigned int, b, c); \
    FMT_NUM_S("l"  cmd,        long,      long,      unsigned long, b, c); \
    FMT_NUM_S("z"  cmd,     ssize_t,   ssize_t,             size_t, b, c); \
    FMT_NUM_S("t"  cmd,   ptrdiff_t, ptrdiff_t,          uintmax_t, b, c); \
    FMT_NUM_S("j"  cmd,    intmax_t,  intmax_t,          uintmax_t, b, c)
#define FMT_ALL_NUM_U(cmd, b, c, h1, h2)                                       \
    FMT_NUM_U("hh" cmd,      unsigned char,       unsigned int, b, c, h1, h2); \
    FMT_NUM_U("h"  cmd,     unsigned short,       unsigned int, b, c, h1, h2); \
    FMT_NUM_U(     cmd,       unsigned int,       unsigned int, b, c, h1, h2); \
    FMT_NUM_U("l"  cmd,      unsigned long,      unsigned long, b, c, h1, h2); \
    FMT_NUM_U("ll" cmd, unsigned long long, unsigned long long, b, c, h1, h2); \
    FMT_NUM_U("z"  cmd,             size_t,            ssize_t, b, c, h1, h2); \
    FMT_NUM_U("t"  cmd,          uintmax_t,          ptrdiff_t, b, c, h1, h2); \
    FMT_NUM_U("j"  cmd,          uintmax_t,          uintmax_t, b, c, h1, h2)

/* %n etc. is different to the return value, in that it's only the amount of
 *  data that has been "output" */
#define FMT_RET_N(strT, T) do {                 \
      if (match_cmd(&fmt, strT "n"))            \
      {                                         \
        unsigned int msz = !sz ? 0 : (sz - 1);  \
                                                \
        T *tmp_ret = va_arg(ap, T *);           \
        *tmp_ret = ((ret > msz) ? msz : ret);   \
        ++fmt;                                  \
        continue;                               \
      } } while (0)

/* main entry point ... */
unsigned int simple_vsnprintf(char *buf, unsigned int sz,
                              const char *fmt, va_list ap)
{
  unsigned int ret = 0;
  
  while (*fmt)
  {
    unsigned int wid = 0;
    int prec  = -1;
    int flg_zero  = 0;
    int flg_hash  = 0;
    int flg_minus = 0;
    int flg_plus  = 0;
    int flg_spac  = 0;
    int flg_parse = 1;
  
    if ((*fmt != '%') || (*++fmt == '%'))
    {
      ADD_C(*fmt++);
      continue;
    }

    /* write out how much data has been written */
    FMT_RET_N("hh", signed char);
    FMT_RET_N("h",  short);
    FMT_RET_N("",   int);
    FMT_RET_N("l",  long);
    FMT_RET_N("ll", long long);
    FMT_RET_N("z",  ssize_t);
    FMT_RET_N("t",  ptrdiff_t);
    FMT_RET_N("j",  intmax_t);

    /* parse the flags for the data */
    while (*fmt && flg_parse)
      switch (*fmt)
      {
        case '0': flg_zero  = 1; ++fmt; break;
        case '#': flg_hash  = 1; ++fmt; break;
        case '-': flg_minus = 1; ++fmt; break;
        case '+': flg_plus  = 1; ++fmt; break;
        case ' ': flg_spac  = 1; ++fmt; break;
        default:  flg_parse = 0;        break;
      }
    
    if (match_cmd(&fmt, "*"))
    {
      int tmp = va_arg(ap, int); wid = tmp;
      if (tmp < 0) wid = -wid;
    }
    else
      PARSE_FMT_NUM10(wid);
    
    if (match_cmd(&fmt, ".*"))
      prec = va_arg(ap, int);
    else if (match_cmd(&fmt, "."))
    {
      prec = 0;
      PARSE_FMT_NUM10(prec);
    }
    
    if (0) { /* not reached */ }
    /* a C style string type, limited by precision */
    else if (match_cmd(&fmt, "s"))
    {
      const char *arg = va_arg(ap, char *);
      const char *tmp = arg;
      unsigned int len = 0;
      
      if (!arg) arg = "";
      else while (*tmp++) ++len;

      if (prec > 0)
      {  
        if (len > (unsigned int)prec) len = prec;
        if (wid > (unsigned int)prec) wid = prec;
      }

      DEC_WIDTH(len);
      
      if (!flg_minus) ADD_PAD_C(' ', wid);
      
      while (len--) ADD_C(*arg++);
      
      ADD_PAD_C(' ', wid);
    }
    /* just a simple character */
    else if (match_cmd(&fmt, "c")) ADD_C(va_arg(ap, int));
    /* the numbers, including all the sized variations... */
    FMT_ALL_NUM_S("d", 10, "0123456789");
    FMT_ALL_NUM_S("i", 10, "0123456789");
    FMT_ALL_NUM_U("u", 10, "0123456789",         0,   0);
    FMT_ALL_NUM_U("o",  8, "01234567",         '0',   0);
    FMT_ALL_NUM_U("x", 16, "0123456789abcdef", '0', 'x');
    FMT_ALL_NUM_U("X", 16, "0123456789ABCDEF", '0', 'X');
    /* this isn't that portable, but it will work most places... and
    * everyone needs %p :) */
    else { flg_hash = 1; if (0) { }
    FMT_NUM_U("p", uintptr_t, void *, 16, "0123456789ABCDEF", '0', 'x');
    else break; }
  }
  
  if (sz) *buf = 0;

  return (ret);
}

unsigned int simple_snprintf(char *buf, unsigned int sz, const char *fmt, ...)
{
  va_list ap;
  unsigned int ret = 0;
  
  va_start(ap, fmt);
  ret = simple_vsnprintf(buf, sz, fmt, ap);
  va_end(ap);

  return (ret);
}

/* note that we can't do simple_vasprintf() without re-implementing or
 * changing the main entry point to limit or allocate ...
 * or using a va_copy() like call (implementation left as an exersise for
 * the reader :) */
unsigned int simple_asprintf(char **buf, const char *fmt, ...)
{
  va_list ap;
  unsigned int ret = 0;
  
  va_start(ap, fmt);
  ret = simple_vsnprintf(0, 0, fmt, ap);
  va_end(ap);

  if (!(*buf = malloc(++ret)))
    return (0);
  
  va_start(ap, fmt);
  ret = simple_vsnprintf(*buf, ret, fmt, ap);
  va_end(ap);

  return (ret);
}

/* this is what 99% of people want to use a lot of the time, also it does the
 * malloc/free instead of realloc so that you can pass the old value as an
 * argument ... which is often desired. */
unsigned int simple_append_asprintf(char **buf, unsigned int *len,
                                    const char *fmt, ...)
{
  va_list ap;
  unsigned int ret = 0;
  char *tmp = NULL;
  
  va_start(ap, fmt);
  ret = simple_vsnprintf(0, 0, fmt, ap);
  va_end(ap);

  if (!(tmp = malloc(*len + ++ret)))
    return (0);
  memcpy(tmp, *buf, *len);
  
  va_start(ap, fmt);
  ret = simple_vsnprintf(tmp + *len, ret, fmt, ap);
  va_end(ap);

  free(*buf);
  
  *buf  = tmp;
  *len += ret;
  
  return (*len);
}

/* some random test code... */
#include <stdio.h>
#include <ctype.h>
#include <locale.h>

int main(void)
{
  char *buf = NULL;
  unsigned int len = 0;
  unsigned int scan = 0;
  intmax_t nnum = 0;

  setlocale(LC_ALL, "");
  
  scan = simple_asprintf(&buf,
                         "<"
                         "%s|%.2s|"
                         "%+04d|%+8.4d|%04.8d|%+08.d|"
                         "%#-8llx|"
                         "%c|%08.5o|"
                         "%p"
                         ">%jn",
                         "abcd", "abcd",
                         4, 4, 4, 4,
                         88LL,
                         '-', 444,
                         (void *)&nnum, &nnum);
  puts(buf);
  simple_snprintf(buf, scan, "%u=%jd\n", scan, nnum);
  puts(buf);
  free(buf); buf = NULL;

  /* simple appending of data */
  scan = 0;
  while (scan++ < 9)
    simple_append_asprintf(&buf, &len, "%d", scan);

  scan = 0;
  while (++scan <= 3)
    simple_append_asprintf(&buf, &len, " %s", buf);

  puts(buf);
  
  len  = 0;

  simple_append_asprintf(&buf, &len, "%s", "    ");
  scan = 0;
  while (scan <= 15)
    simple_append_asprintf(&buf, &len, " %x ", scan++);

  scan = 0;
  while (scan < 256)
  {
    if (!(scan % 16)) simple_append_asprintf(&buf, &len, "\n%03x ", scan);
    
    simple_append_asprintf(&buf, &len, "<%c>", isprint(scan) ? scan : '?');

    ++scan;
  }

  puts(buf);

  free(buf);
  
  exit (EXIT_SUCCESS);
}
