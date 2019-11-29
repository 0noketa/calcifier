#ifndef calcifier_rtl__h
#define calcifier_rtl__h

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <stdbool.h>

#define createElseable calcifier_createElseable
#define tryTrim calcifier_tryTrim
#define find calcifier_find
#define findAny calcifier_findAny
#define findr calcifier_findr
#define findrAny calcifier_findrAny
#define tryParseInt calcifier_tryParseInt

typedef struct {
    bool success;
    int value;
} Elseable;


static inline Elseable createElseable(bool success, int value) {
    Elseable r = { .success = success, .value = value };

    return r;
}

extern bool tryTrim(char *s, ptrdiff_t len, char **out_s, ptrdiff_t *out_len);
extern ptrdiff_t find(const char *s, ptrdiff_t len, int ch);
extern ptrdiff_t findAny(const char *s, ptrdiff_t len, const char *cs);
extern ptrdiff_t findr(const char *s, ptrdiff_t len, int ch);
extern ptrdiff_t findrAny(const char *s, ptrdiff_t len, const char *cs);

extern bool tryParseInt(char *s, ptrdiff_t len, int *out_value);


#endif
