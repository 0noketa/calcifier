#include "calcifier_rtl.h"


int var_it;
int vars[256];

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


bool tryParseName(char *s, ptrdiff_t len, int *out_value) {
    if (s == NULL || !*s || len == 0) return false;

    char *s2;
    ptrdiff_t len2;
    tryTrim(s, len, &s2, &len2);

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
    tryTrim(s, len, &s2, &len2);

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
        ptrdiff_t opr = findrAny(s, len2, "*/%");

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
        ptrdiff_t opr = findrAny(s, len2, "+-");

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
        ptrdiff_t opr = findrAny(s, len2, "=#<>");

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
