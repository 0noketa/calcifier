/* for output of calcifier */
#include "calcifier_rtl.h"


bool tryTrim(char *s, ptrdiff_t len, char **out_s, ptrdiff_t *out_len) {
    if (s == NULL || !*s) {
        if (out_s) *out_s = s;
        if (out_len) *out_len = 0;

        return false;
    }

    ptrdiff_t i;
    for (i = 0; i < len && strchr(" \t\n\r", s[i]); ++i);

    ptrdiff_t j;
    for (j = len; j - 1 > i && strchr(" \t\n\r", s[j - 1]); --j);

    if (j <= i) i = j = 0;

    if (out_s) *out_s = s + i;
    if (out_len) *out_len = j - i;

    return true;
}

ptrdiff_t find(const char *s, ptrdiff_t len, int ch) {
    char *p = strchr(s, ch);

    return p != NULL ? p - s : len;
}

ptrdiff_t findAny(const char *s, ptrdiff_t len, const char *cs) {
    for (ptrdiff_t i = 0; i < len; ++i) {
        if (strchr(cs, s[i])) {

            return i;
        }
    }

    return len;
}

ptrdiff_t findr(const char *s, ptrdiff_t len, int ch) {
    for (ptrdiff_t i = len; i-- > 0;) {
        if (s[i] == ch) {
            return i;
        }
    }

    return len;
}

ptrdiff_t findrAny(const char *s, ptrdiff_t len, const char *cs) {
    for (ptrdiff_t i = len; i-- > 0;) {
        if (strchr(cs, s[i])) {

            return i;
        }
    }

    return len;
}


static bool validateInt(char *s, ptrdiff_t len) {
    if (s == NULL || !*s || len == 0) return false;

    ptrdiff_t i;

    for (i = 0; i < len && strchr("+-", len); ++i);
    for (; i < len && isdigit(s[i]); ++i);

    return i >= len;
}

bool tryParseInt(char *s, ptrdiff_t len, int *out_value) {
    if (s == NULL || !*s || len == 0) return false;

    const int lim = 256;
    char buf[256];
    
    if (len > lim - 1) len = lim - 1;

    char *s2;
    ptrdiff_t len2;
    tryTrim(s, len, &s2, &len2);

    if (validateInt(s2, len2)) {
        strncpy(buf, s2, len2);
        buf[len] = 0;
        
        if (out_value) *out_value = atoi(buf);

        return true;
    } else {
        return false;
    }
}

