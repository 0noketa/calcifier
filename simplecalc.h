#ifndef simplecalc__h
#define simplecalc__h

#include "calcifier_rtl.h"

typedef int simplecalc_node_t;


extern bool simplecalc_tryParseValue(
    char *s, ptrdiff_t len,
    simplecalc_node_t *out_value);

/* main */
extern bool simplecalc_tryParse(char *s, ptrdiff_t len, simplecalc_node_t *out_value);

extern ptrdiff_t simplecalc_skipl(char *s, ptrdiff_t len, ptrdiff_t i);
extern ptrdiff_t simplecalc_skipr(char *s, ptrdiff_t len, ptrdiff_t i);

extern simplecalc_node_t simplecalc_nilnode(void);
extern simplecalc_node_t simplecalc_delnode(simplecalc_node_t x);

extern simplecalc_node_t simplecalc_newnode_nop_1(simplecalc_node_t x, simplecalc_node_t y);
extern simplecalc_node_t simplecalc_newnode_neg_1(simplecalc_node_t x, simplecalc_node_t y);
extern simplecalc_node_t simplecalc_newnode_not_1(simplecalc_node_t x, simplecalc_node_t y);
extern simplecalc_node_t simplecalc_newnode_abs_1(simplecalc_node_t x, simplecalc_node_t y);
extern simplecalc_node_t simplecalc_newnode_add_2(simplecalc_node_t x, simplecalc_node_t y);
extern simplecalc_node_t simplecalc_newnode_sub_2(simplecalc_node_t x, simplecalc_node_t y);
extern simplecalc_node_t simplecalc_newnode_mul_2(simplecalc_node_t x, simplecalc_node_t y);
extern simplecalc_node_t simplecalc_newnode_div_2(simplecalc_node_t x, simplecalc_node_t y);
extern simplecalc_node_t simplecalc_newnode_mod_2(simplecalc_node_t x, simplecalc_node_t y);
extern simplecalc_node_t simplecalc_newnode_equ_2(simplecalc_node_t x, simplecalc_node_t y);
extern simplecalc_node_t simplecalc_newnode_neq_2(simplecalc_node_t x, simplecalc_node_t y);
extern simplecalc_node_t simplecalc_newnode_lt_2(simplecalc_node_t x, simplecalc_node_t y);
extern simplecalc_node_t simplecalc_newnode_gt_2(simplecalc_node_t x, simplecalc_node_t y);


#endif
