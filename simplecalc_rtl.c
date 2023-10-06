/* for output of calcifier */
#include "calcifier_rtl.h"
#include "simplecalc.h"

#define MY(s) simplecalc_##s

typedef MY(node_t) node_t;


static node_t newnode(char *s) { return atoi(s); }

node_t MY(nilnode)(void) { return 0; }
node_t MY(delnode)(node_t x) { return 0; }


#define NODEN1(name) \
node_t MY(newnode_##name)(node_t x, node_t y)

#define NODEN2(name) \
node_t MY(newnode_##name)(node_t x, node_t y)


NODEN1(nop_1) { return x; }
NODEN1(neg_1) { return -x; }
NODEN1(not_1) { return ~x; }
NODEN1(abs_1) { return abs(x); }

NODEN2(add_2) { return x + y; }
NODEN2(sub_2) { return x - y; }
NODEN2(mul_2) { return x * y; }
NODEN2(div_2) { return y ? x / y : 0; }
NODEN2(mod_2) { return y ? x % y : 0; }
NODEN2(equ_2) { return x == y; }
NODEN2(neq_2) { return x != y; }
NODEN2(lt_2) { return x < y; }
NODEN2(gt_2) { return x > y; }


static bool validateInt(char *s, ptrdiff_t len) {
    if (s == NULL || !*s || len == 0) return false;

    ptrdiff_t i;

    for (i = 0; i < len && strchr("+-", len); ++i);
    for (; i < len && isdigit(s[i]); ++i);

    return i >= len;
}

bool MY(tryParseValue)(char *s, ptrdiff_t len, node_t *out_value) {
    if (s == NULL || !*s || len == 0) return false;

    const int lim = 256;
    char buf[256];
    
    if (len > lim - 1) len = lim - 1;

    if (validateInt(s, len)) {
        strncpy(buf, s, len);
        buf[len] = 0;
        
        if (out_value)
            *out_value = newnode(buf);

        return true;
    } else {
        return false;
    }
}

ptrdiff_t MY(skipl)(char *s, ptrdiff_t len, ptrdiff_t i) {
    return calcifier_skipl(s, len, i);
}
ptrdiff_t MY(skipr)(char *s, ptrdiff_t len, ptrdiff_t i) {
    return calcifier_skipr(s, len, i);
}

