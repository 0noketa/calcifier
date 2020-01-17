#ifndef calcifier_rtl__h
#define calcifier_rtl__h

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <stdbool.h>

#define tryTrim calcifier_tryTrim


extern bool tryTrim(char *s, ptrdiff_t len, char **out_s, ptrdiff_t *out_len);

extern ptrdiff_t calcifier_skipl(char *s, ptrdiff_t len, ptrdiff_t i);
extern ptrdiff_t calcifier_skipr(char *s, ptrdiff_t len, ptrdiff_t i);


#endif
