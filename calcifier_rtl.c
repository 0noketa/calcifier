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

ptrdiff_t calcifier_skipl(char *s, ptrdiff_t len, ptrdiff_t i) {
    if (!s || i > len || i < 0) return len;

    return i + 1;
}

ptrdiff_t calcifier_skipr(char *s, ptrdiff_t len, ptrdiff_t i) {
    if (!s || i > len || i < 0) return 0;

    return i - 1;
}
