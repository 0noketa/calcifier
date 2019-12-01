/* for output of calcifier */
#include "calcifier_rtl.h"
#include "calcifier_calc.h"

#define MY(s) calcifier_calc_##s
#define E(s) CALCIFIER_CALC_##s

typedef MY(nodetype_t) nodetype_t;
typedef MY(node_t) node_t;
typedef MY(value_t) value_t;


static value_t vars[256];
static value_t var_it;


node_t *MY(nilnode)(void) { return NULL; }
node_t *MY(delnode)(node_t *x) {
    if (x != NULL) {
        MY(delnode)(x->_left);
        MY(delnode)(x->_right);
        free(x);
    }

    return NULL;
}

static node_t *newnode(
    nodetype_t _type,
    int _value,
    node_t *_left,
    node_t *_right,
    value_t (*_eval)(node_t*),
    value_t (*_func)(value_t, value_t))
{
    if (_left && _left->_type == E(NODE_ERROR)) {
        calcifier_calc_delnode(_right);
        return _left;
    }
    if (_right && _right->_type == E(NODE_ERROR)) {
        calcifier_calc_delnode(_left);
        return _right;
    }

    node_t *r = malloc(sizeof(node_t));
    if (!r) return NULL;

    node_t v = { _type, _value, _left, _right, _eval, _func };
    *r = v;

    return r;
}

static value_t eval_error(node_t *x);
static value_t func_error(value_t x, value_t y);
static node_t *subnode(
    nodetype_t _type,
    node_t *_left,
    node_t *_right)
{
    return newnode(_type, 0, _left, _right, eval_error, func_error);
}


value_t MY(eval)(node_t *x) {
    return x ? x->_eval(x) : 0;
}

static value_t eval_error(node_t *x) {
printf("error %p->(%p, %p)\n", x, x->_left, x->_right);
    return 0;
}
static value_t eval_it(node_t *x) {
    return x->_type == E(NODE_IT) ? var_it : 0;
}
static value_t eval_value(node_t *x) {
    return x->_type == E(NODE_VALUE) ? x->_value : 0;
}
static value_t eval_unary(node_t *x) {
    return x->_type == E(NODE_UNARY) ? x->_func(MY(eval)(x->_left), 0) : 0;
}
static value_t eval_binary(node_t *x) {
    return x->_type == E(NODE_BINARY) ? x->_func(MY(eval)(x->_left), MY(eval)(x->_right)) : 0;
}
static value_t eval_else(node_t *x) {
    return x->_type == E(NODE_ELSE)
        && x->_left->_type == E(NODE_THEN)
        ? MY(eval)(x->_left->_left)
            ? MY(eval)(x->_left->_right)
            : MY(eval)(x->_right)
        : 0;
}
static value_t eval_while(node_t *x) {
    if (x->_type != E(NODE_WHILE)) return 0;

    value_t r = 0;
    while (MY(eval)(x->_left))
        r = MY(eval)(x->_right);

    return r;
}
static value_t eval_do(node_t *x) {
    if (x->_type != E(NODE_DO)) return 0;

    var_it = MY(eval)(x->_left);
    return MY(eval)(x->_right);
}

static value_t func_error(value_t x, value_t y) { return 0; }


#define NODEV1(name) \
value_t MY(func_##name)(value_t x, value_t y); \
node_t *MY(newnode_##name)(node_t *x, node_t *y) \
{ return newnode(E(NODE_UNARY), 0, x, y, eval_unary, MY(func_##name)); } \
value_t MY(func_##name)(value_t x, value_t y)

#define NODEV2(name) \
value_t MY(func_##name)(value_t x, value_t y); \
node_t *MY(newnode_##name)(node_t *x, node_t *y) \
{ return newnode(E(NODE_BINARY), 0, x, y, eval_binary, MY(func_##name)); } \
value_t MY(func_##name)(value_t x, value_t y)

#define NODEN1(name) \
node_t *MY(newnode_##name)(node_t *x, node_t *y)

#define NODEN2(name) \
node_t *MY(newnode_##name)(node_t *x, node_t *y)

#define NODE2(name, nodetype, eval, func) \
NODEN2(name) \
{ return newnode(nodetype, 0, x, y, eval, func); }

#define SNODE2(name, nodetype) \
NODEN2(name) \
{ return subnode(nodetype, x, y); }


NODEV1(nop_1) { return x; }
NODEV1(setit_1) { vars[abs(x % 256)] = var_it; return var_it; }
NODEV1(get_1) { return vars[abs(x % 256)]; }
NODEV1(neg_1) { return -x; }
NODEV1(not_1) { return ~x; }
NODEV1(abs_1) { return abs(x); }

NODEV1(set_2) { vars[abs(x % 256)] = y; return y; }
NODEV2(add_2) { return x + y; }
NODEV2(sub_2) { return x - y; }
NODEV2(mul_2) { return x * y; }
NODEV2(div_2) { return y ? x / y : 0; }
NODEV2(mod_2) { return y ? x % y : 0; }
NODEV2(equ_2) { return x == y; }
NODEV2(neq_2) { return x != y; }
NODEV2(lt_2) { return x < y; }
NODEV2(gt_2) { return x > y; }

SNODE2(then_2, E(NODE_THEN))
NODEN2(pyif_2) { return subnode(E(NODE_THEN), y, x); }
NODEN2(else_2) { return newnode(E(NODE_ELSE), 0, x, y, eval_else, func_error); }
NODEN2(while_2) { return newnode(E(NODE_WHILE), 0, x, y, eval_while, func_error); }
NODEN2(do_2) { return newnode(E(NODE_DO), 0, x, y, eval_do, func_error); }

node_t *MY(newnode_name_0)(char *s, ptrdiff_t len) {
    return newnode(E(NODE_VALUE), abs(*s) % 256, NULL, NULL, eval_value, MY(func_nop_1));
}
node_t *MY(newnode_it_0)(void) {
    return newnode(E(NODE_IT), 0, NULL, NULL, eval_it, func_error);
}




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

ptrdiff_t MY(skipl)(char *s, ptrdiff_t len, ptrdiff_t i) {
    if (!s || i > len || i < 0) return len;

    return i + 1;
}

ptrdiff_t MY(skipr)(char *s, ptrdiff_t len, ptrdiff_t i) {
    if (!s || i > len || i < 0) return 0;

    return i - 1;
}

static bool validateInt(char *s, ptrdiff_t len) {
    if (s == NULL || !*s || len == 0) return false;

    ptrdiff_t i;

    for (i = 0; i < len && strchr("+-", len); ++i);
    for (; i < len && isdigit(s[i]); ++i);

    return i >= len;
}

bool MY(tryParseValue)(char *s, ptrdiff_t len, node_t **out_value) {
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
        
        if (out_value)
            *out_value = newnode(E(NODE_VALUE), atoi(buf),
                NULL, NULL, eval_value, MY(func_nop_1));

        return true;
    } else {
        return false;
    }
}
