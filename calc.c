#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    bool success;
    int value;
} Elseable;


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
            while (i > 0 && strchr(cs, s[i - 1])) --i;

            return i;
        }
    }

    return len;
}

int parseInt(char *s, ptrdiff_t len);
int parseMul(char *s, ptrdiff_t len);
int parseAdd(char *s, ptrdiff_t len);
int parseEq(char *s, ptrdiff_t len);
int parseElse(char *s, ptrdiff_t len);

int parseInt(char *s, ptrdiff_t len) {
    const int lim = 256;
//    char buf[len + 1];
    char buf[256];
    
    if (len > lim - 1) len = lim - 1;

    char *s2;
    ptrdiff_t len2;
    trim(s, len, &s2, &len2);
    
    strncpy(buf, s2, len2);
    buf[len] = 0;
    
    return atoi(buf);
}

int parseSign(char *s, ptrdiff_t len) {
    char *s2;
    ptrdiff_t len2;
    trim(s, len, &s2, &len2);

    if (len2 > 0 && strchr("+-=#*/%", s2[0])) {
        int value = parseSign(s2 + 1, len2 - 1);

        return s2[0] == '-' ? -value
            : /* others */ value;
    } else {
        return parseInt(s2, len2);
    }
}

int parseMul(char *s, ptrdiff_t len) {
    ptrdiff_t opr = findr_any(s, len, "*/%");
    
    if (opr > 0 && opr < len - 1) {
        int left = parseMul(s, opr);

        char *right_p = s + opr + 1;  // '*'の次
        ptrdiff_t right_len = len - opr - 1;  // '*'の次から末尾までの長さ

        int right = parseSign(right_p, right_len);
        
        return s[opr] == '*' ? left * right
            : right == 0 ? 0
            : s[opr] == '/' ? left / right
            : /* s[opr] == '%' ? */ left % right;
    } else {
        return parseSign(s, len);
    }
}

int parseAdd(char *s, ptrdiff_t len) {
    ptrdiff_t opr = findr_any(s, len, "+-");
    
    if (opr > 0 && opr < len - 1) {
        int left = parseAdd(s, opr);

        char *right_p = s + opr + 1;
        ptrdiff_t right_len = len - opr - 1;
        int right = parseMul(right_p, right_len);

        return s[opr] == '+' ? left + right
            : /* s[opr] == '-' ? */ left - right;
    } else {
        return parseMul(s, len);
    }
}

int parseEq(char *s, ptrdiff_t len) {
    ptrdiff_t opr = findr_any(s, len, "=#<>");
    
    if (opr > 0 && opr < len - 1) {
        int left = parseEq(s, opr);

        char *right_p = s + opr + 1;
        ptrdiff_t right_len = len - opr - 1;
        int right = parseAdd(right_p, right_len);

        return s[opr] == '=' ? left == right
            : s[opr] == '#' ? left != right
            : s[opr] == '<' ? left < right
            : /* s[opr] == '>' ? */ left > right;
    } else {
        return parseAdd(s, len);
    }
}


static Elseable parseThen(char *s, ptrdiff_t len) {
    ptrdiff_t opr = find(s, len, '?');
    
    if (opr < len) {
        int left = parseEq(s, opr);

        char *right_p = s + opr + 1;
        ptrdiff_t right_len = len - opr - 1;
        int right = parseEq(right_p, right_len);

        Elseable result = { .success = left, .value = right };

        return result;
    } else {
        int left = parseEq(s, len);
        Elseable result = { .success = left, .value = left };

        return result;
    }
}

int parseElse(char *s, ptrdiff_t len) {
    ptrdiff_t opr = find(s, len, ':');
    
    if (opr < len) {
        Elseable left = parseThen(s, opr);

        if (left.success) return left.value; 

        char *right_p = s + opr + 1;
        ptrdiff_t right_len = len - opr - 1;
        int right = parseElse(right_p, right_len);

        return right;
    } else {
        return parseEq(s, len);
    }
}


int main(int argc, char *argv[]) {
    char *expr = "1+2*3+4";

    if (argc > 1) expr = argv[1];

    printf("%s = %d\n", expr, parseElse(expr, strlen(expr)));

    return 0;
}
