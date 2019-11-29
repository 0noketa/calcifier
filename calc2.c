#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    bool success;
    int value;
} Elseable;


int var_it;
int vars[26];


static Elseable createElseable(bool success, int value) {
    Elseable r = { .success = success, .value = value };

    return r;
}

bool trim(char *s, ptrdiff_t len, char **out_s, ptrdiff_t *out_len) {
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


// 左から探す。無ければ長さを返す。右再帰用。returns index of ch if s = {..s0, ch, ..s1} (s1はch含まず)
ptrdiff_t find(const char *s, ptrdiff_t len, int ch) {
    char *p = strchr(s, ch);

    return p != NULL ? p - s : len;
}

// 右から探す。左再帰用。 returns index of ch if s = {..s0, ch, ..s1} (s0はch含まず)
ptrdiff_t findr(const char *s, ptrdiff_t len, int ch) {
    for (ptrdiff_t i = len; i-- > 0;) {
        if (s[i] == ch) {
            while (i > 0 && s[i - 1] == ch) --i;

            return i;
        }
    }

    return len;
}
// 左再帰用。複数指定可。
ptrdiff_t findr_any(const char *s, ptrdiff_t len, const char *cs) {
    for (ptrdiff_t i = len; i-- > 0;) {
        if (strchr(cs, s[i])) {

            return i;
        }
    }

    return len;
}

bool tryParseInt(char *s, ptrdiff_t len, int *out_value);
bool tryParseName(char *s, ptrdiff_t len, int *out_value);
bool tryParseSign(char *s, ptrdiff_t len, int *out_value);
bool tryParseMul(char *s, ptrdiff_t len, int *out_value);
bool tryParseAdd(char *s, ptrdiff_t len, int *out_value);
bool tryParseEq(char *s, ptrdiff_t len, int *out_value);
bool tryParseElse(char *s, ptrdiff_t len, int *out_value);
bool tryParseDo2(char *s, ptrdiff_t len, int *out_value);
bool tryParseWhile(char *s, ptrdiff_t len, int *out_value);
bool tryParseDo(char *s, ptrdiff_t len, int *out_value);
bool tryParseExpr(char *s, ptrdiff_t len, int *out_value);

bool validateInt(char *s, ptrdiff_t len) {
    if (s == NULL || !*s || len == 0) return false;

    ptrdiff_t i;

    for (i = 0; i < len && strchr("+-", len); ++i);
    for (; i < len && isdigit(s[i]); ++i);

    return i >= len;
}

bool tryParseInt(char *s, ptrdiff_t len, int *out_value) {
    if (s == NULL || !*s || len == 0) return false;

    const int lim = 256;
//    char buf[len + 1];
    char buf[256];
    
    if (len > lim - 1) len = lim - 1;

    char *s2;
    ptrdiff_t len2;
    trim(s, len, &s2, &len2);

    if (validateInt(s2, len2)) {
        strncpy(buf, s2, len2);
        buf[len] = 0;
        
        if (out_value) *out_value = atoi(buf);

        return true;
    } else {
        return false;
    }
}

bool tryParseName(char *s, ptrdiff_t len, int *out_value) {
    if (s == NULL || !*s || len == 0) return false;

    char *s2;
    ptrdiff_t len2;
    trim(s, len, &s2, &len2);

    if (len2 == 1 && islower(*s2)) {
        if (out_value) *out_value = *s2 - 'a';

        return true;
    } else if (len2 == 2 && strncmp(s2, "it", 2) == 0) {
        if (out_value) *out_value = var_it;

        return true;
    }

    return false;
}

bool tryParseSign(char *s, ptrdiff_t len, int *out_value) {
    char *s2;
    ptrdiff_t len2;
    trim(s, len, &s2, &len2);

    if (len2 == 0) return false;

    if (strchr("+-=#*/%", s2[0])
        && tryParseSign(s2 + 1, len2 - 1, out_value))
    {
        if (out_value) {
            *out_value = s2[0] == '-' ? -*out_value
                : s2[0] == '*' ? vars[abs(*out_value % 26)]
                : s2[0] == '=' ? (vars[abs(*out_value % 26)] = var_it)
                : *out_value;
        }

        return true;
    } else {
        return tryParseInt(s2, len2, out_value)
            || tryParseName(s2, len2, out_value);
    }
}

bool tryParseMul(char *s, ptrdiff_t len, int *out_value) {
    for (ptrdiff_t len2 = len; len2 > 0; --len2) {
        ptrdiff_t opr = findr_any(s, len2, "*/%");

        if (opr >= len2) continue;

        char *right_p = s + opr + 1;  // '*'の次
        ptrdiff_t right_len = len - opr - 1;  // '*'の次から末尾までの長さ

        int left, right;

        if (tryParseMul(s, opr, &left)
            && tryParseSign(right_p, right_len, &right))
        {
            if (out_value) {
                *out_value = s[opr] == '*' ? left * right
                    : right == 0 ? 0
                    : s[opr] == '/' ? left / right
                    : /* s[opr] == '%' ? */ left % right;
            }

            return true;
        }            
    }

    return tryParseSign(s, len, out_value);
}

bool tryParseAdd(char *s, ptrdiff_t len, int *out_value) {
    for (ptrdiff_t len2 = len; len2 > 0; --len2) {
        ptrdiff_t opr = findr_any(s, len2, "+-");

        if (opr >= len2) continue;

        char *right_p = s + opr + 1;  // '*'の次
        ptrdiff_t right_len = len - opr - 1;  // '*'の次から末尾までの長さ

        int left, right;

        if (tryParseAdd(s, opr, &left)
            && tryParseMul(right_p, right_len, &right))
        {
            if (out_value) {
                *out_value = s[opr] == '+' ? left + right
                    : /* s[opr] == '-' ? */ left - right;
            }

            return true;
        }            
    }

    return tryParseMul(s, len, out_value);
}

bool tryParseEq(char *s, ptrdiff_t len, int *out_value) {
    for (ptrdiff_t len2 = len; len2 > 0; --len2) {
        ptrdiff_t opr = findr_any(s, len2, "=#<>");

        if (opr >= len2) continue;

        char *right_p = s + opr + 1;  // '*'の次
        ptrdiff_t right_len = len - opr - 1;  // '*'の次から末尾までの長さ

        int left, right;

        if (tryParseEq(s, opr, &left)
            && tryParseAdd(right_p, right_len, &right))
        {
            if (out_value) {
                *out_value = s[opr] == '=' ? left == right
                    : s[opr] == '#' ? left != right
                    : s[opr] == '<' ? left < right
                    : /* s[opr] == '>' ? */ left > right;
            }

            return true;
        }            
    }

    return tryParseAdd(s, len, out_value);
}

static bool tryParseThen(char *s, ptrdiff_t len, Elseable *out_result) {
    ptrdiff_t opr = find(s, len, '?');
    
    if (opr < len) {
        int left, right;
        
        char *right_p = s + opr + 1;
        ptrdiff_t right_len = len - opr - 1;

        if (tryParseEq(s, opr, &left)
            && tryParseEq(right_p, right_len, &right))
        {
            if (out_result) *out_result = createElseable(left, right);

            return true;
        }
    } else {
        int left;
        
        if (tryParseEq(s, len, &left)) {
            if (out_result) *out_result = createElseable(left, left);

            return true;
        }
    }

    if (out_result) *out_result = createElseable(false, 0);

    return false;
}

bool tryParseElse(char *s, ptrdiff_t len, int *out_value) {
    ptrdiff_t opr = find(s, len, ':');
    
    if (opr < len) {
        Elseable left;
        
        if (!tryParseThen(s, opr, &left)) return false;

        if (left.success) {
            if (out_value) *out_value = left.value;

            return true;
        }

        char *right_p = s + opr + 1;
        ptrdiff_t right_len = len - opr - 1;
        int right;
        
        if (!tryParseElse(right_p, right_len, &right)) return false;
        
        if (out_value) *out_value = right;

        return true;
    } else {
        return tryParseEq(s, len, out_value);
    }
}

bool tryParseDo2(char *s, ptrdiff_t len, int *out_value) {
    ptrdiff_t opr = find(s, len, ',');
    
    if (opr < len) {
        int left;

        if (!tryParseElse(s, opr, &left)) return false;

        var_it = left;

        char *right_p = s + opr + 1;
        ptrdiff_t right_len = len - opr - 1;
        
        return tryParseDo2(right_p, right_len, out_value);
    } else {
        int left;

        if (tryParseElse(s, len, &left)) {
            if (out_value) *out_value = var_it = left;

            return true;
        } else {
            return false;
        }
    }
}

bool tryParseWhile(char *s, ptrdiff_t len, int *out_value) {
    ptrdiff_t opr = find(s, len, '@');
    
    if (opr < len) {
        char *right_p = s + opr + 1;
        ptrdiff_t right_len = len - opr - 1;
        int left, right;
        
        while (true) {
            if (!tryParseDo2(s, opr, &left)) return false;

            if (!left) break;
            
            if (!tryParseWhile(right_p, right_len, &right)) return false;
            
            if (out_value) *out_value = right;
        }

        return true;
    } else {
        return tryParseDo2(s, len, out_value);
    }
}

bool tryParseDo(char *s, ptrdiff_t len, int *out_value) {
    ptrdiff_t opr = find(s, len, ';');
    
    if (opr < len) {
        int left;

        if (!tryParseWhile(s, opr, &left)) return false;

        var_it = left;

        char *right_p = s + opr + 1;
        ptrdiff_t right_len = len - opr - 1;
        
        return tryParseDo(right_p, right_len, out_value);
    } else {
        int left;

        if (tryParseWhile(s, len, &left)) {
            if (out_value) *out_value = var_it = left;

            return true;
        } else {
            return false;
        }
    }
}

bool tryParseExpr(char *s, ptrdiff_t len, int *out_value) {
    return tryParseDo(s, len, out_value);
}


int main(int argc, char *argv[]) {
    char *expr = "1+2*3+4";

    if (argc > 1) expr = argv[1];

    int result;

    if (!tryParseExpr(expr, strlen(expr), &result)) {
        fputs("error\n", stderr);

        return 1;
    }

    printf("%s = %d\n", expr, result);

    return 0;
}
