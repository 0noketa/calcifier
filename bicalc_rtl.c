/* for output of calcifier */
#include "calcifier_rtl.h"
#include "bicalc.h"

#define MY(s) bicalc_##s

typedef MY(nodev_t) nodev_t;
typedef MY(node_t) node_t;


static node_t newnode(char *s) {
    node_t r = malloc(sizeof(nodev_t));

    if (!r) return NULL;

    mpz_init(r);
    mpz_set_str(r, s, BICALC_BASE);

    return r;
}
node_t MY(nilnode)(void) { return NULL; }
node_t MY(delnode)(node_t x) {
    if (x != NULL) {
        mpz_clear(x);
        free(x);
    }

    return NULL;
}


#define NODEN1(name) \
node_t MY(newnode_##name)(node_t x, node_t y)

#define NODEN2(name) \
node_t MY(newnode_##name)(node_t x, node_t y)


NODEN1(nop_1) { return x; }
NODEN1(neg_1) { mpz_neg(x, x); return x; }
NODEN1(abs_1) { mpz_abs(x, x); return x; }

NODEN2(add_2) { mpz_add(x, x, y); mpz_clear(y); free(y); return x; }
NODEN2(sub_2) { mpz_sub(x, x, y); mpz_clear(y); free(y); return x; }
NODEN2(mul_2) { mpz_mul(x, x, y); mpz_clear(y); free(y); return x; }
NODEN2(div_2) { mpz_div(x, x, y); mpz_clear(y); free(y); return x; }
NODEN2(mod_2) { mpz_mod(x, x, y); mpz_clear(y); free(y); return x; }
NODEN2(equ_2) { int n = !mpz_cmp(x, y); mpz_clear(y); free(y); mpz_set_si(x, n); return x; }
NODEN2(neq_2) { int n = !!mpz_cmp(x, y); mpz_clear(y); free(y); mpz_set_si(x, n); return x; }
NODEN2(lt_2) { int n = mpz_cmp(x, y) < 0; mpz_clear(y); free(y); mpz_set_si(x, n); return x; }
NODEN2(gt_2) { int n = mpz_cmp(x, y) > 0; mpz_clear(y); free(y); mpz_set_si(x, n); return x; }


static bool validateInt(char *s, ptrdiff_t len) {
    if (s == NULL || !*s || len == 0) return false;

    ptrdiff_t i = 0;

    for (; i < len && (isdigit(s[i]) || toupper(s[i]) - 'A' < BICALC_BASE - 10); ++i);

    return i >= len;
}

bool MY(tryParseValue)(char *s, ptrdiff_t len, node_t *out_value) {
    if (s == NULL || !*s || len == 0) return false;

    char *buf = malloc(len + 1);
    
    if (buf && validateInt(s, len)) {
        strncpy(buf, s, len);
        buf[len] = 0;

        if (out_value)
        {
            node_t r = newnode(buf);
            free(buf);

            if (!r) return false;

            *out_value = r;
        }

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

