#ifndef bicalc__h
#define bicalc__h

#include <gmp.h>
#include "calcifier_rtl.h"

typedef MP_INT bicalc_nodev_t;
typedef MP_INT *bicalc_node_t;


extern bool bicalc_tryParseValue(
    char *s, ptrdiff_t len,
    bicalc_node_t *out_value);

/* main */
extern bool bicalc_tryParse(char *s, ptrdiff_t len, bicalc_node_t *out_value);

extern ptrdiff_t bicalc_skipl(char *s, ptrdiff_t len, ptrdiff_t i);
extern ptrdiff_t bicalc_skipr(char *s, ptrdiff_t len, ptrdiff_t i);

extern bicalc_node_t bicalc_nilnode(void);
extern bicalc_node_t bicalc_delnode(bicalc_node_t x);

extern bicalc_node_t bicalc_newnode_nop_1(bicalc_node_t x, bicalc_node_t y);
extern bicalc_node_t bicalc_newnode_neg_1(bicalc_node_t x, bicalc_node_t y);
extern bicalc_node_t bicalc_newnode_abs_1(bicalc_node_t x, bicalc_node_t y);
extern bicalc_node_t bicalc_newnode_add_2(bicalc_node_t x, bicalc_node_t y);
extern bicalc_node_t bicalc_newnode_sub_2(bicalc_node_t x, bicalc_node_t y);
extern bicalc_node_t bicalc_newnode_mul_2(bicalc_node_t x, bicalc_node_t y);
extern bicalc_node_t bicalc_newnode_div_2(bicalc_node_t x, bicalc_node_t y);
extern bicalc_node_t bicalc_newnode_mod_2(bicalc_node_t x, bicalc_node_t y);
extern bicalc_node_t bicalc_newnode_equ_2(bicalc_node_t x, bicalc_node_t y);
extern bicalc_node_t bicalc_newnode_neq_2(bicalc_node_t x, bicalc_node_t y);
extern bicalc_node_t bicalc_newnode_lt_2(bicalc_node_t x, bicalc_node_t y);
extern bicalc_node_t bicalc_newnode_gt_2(bicalc_node_t x, bicalc_node_t y);


#endif
