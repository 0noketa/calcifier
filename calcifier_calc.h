#ifndef calcifier_calc__h
#define calcifier_calc__h

#include "calcifier_rtl.h"

#ifndef CALCIFIER_CALC_VARS
#define CALCIFIER_CALC_VARS "zjycqrslmowbvknxuahpidgeft"
#endif

#ifndef CALCIFIER_CALC_IT
#define CALCIFIER_CALC_IT "it"
#endif


typedef enum CALCIFIER_CALC_NODETYPE_ID {
    CALCIFIER_CALC_NODE_ERROR,
    CALCIFIER_CALC_NODE_VALUE,
    CALCIFIER_CALC_NODE_IT,
    CALCIFIER_CALC_NODE_UNARY,
    CALCIFIER_CALC_NODE_BINARY,
    CALCIFIER_CALC_NODE_THEN,
    CALCIFIER_CALC_NODE_ELSE,
    CALCIFIER_CALC_NODE_WHILE,
    CALCIFIER_CALC_NODE_DO
} calcifier_calc_nodetype_t;

typedef int calcifier_calc_value_t;

typedef struct CALCIFIER_CALC_NODE_ID {
    calcifier_calc_nodetype_t _type;
    calcifier_calc_value_t _value;
    struct CALCIFIER_CALC_NODE_ID *_left, *_right;
    calcifier_calc_value_t (*_eval)(struct CALCIFIER_CALC_NODE_ID*);
    calcifier_calc_value_t (*_func)(calcifier_calc_value_t, calcifier_calc_value_t);
} calcifier_calc_nodev_t;

typedef calcifier_calc_nodev_t *calcifier_calc_node_t;

extern bool calcifier_calc_tryParseValue(
    char *s, ptrdiff_t len,
    calcifier_calc_node_t *out_value);

extern ptrdiff_t calcifier_calc_skipl(char *s, ptrdiff_t len, ptrdiff_t i);
extern ptrdiff_t calcifier_calc_skipr(char *s, ptrdiff_t len, ptrdiff_t i);

/* main */
extern bool calcifier_calc_tryParse(char *s, ptrdiff_t len, calcifier_calc_node_t *out_value);

extern calcifier_calc_value_t calcifier_calc_eval(calcifier_calc_node_t x);

extern calcifier_calc_node_t calcifier_calc_nilnode(void);
extern calcifier_calc_node_t calcifier_calc_delnode(calcifier_calc_node_t x);

extern calcifier_calc_node_t calcifier_calc_newnode_name_0(char *s, ptrdiff_t len);
extern calcifier_calc_node_t calcifier_calc_newnode_it_0(void);

extern calcifier_calc_node_t calcifier_calc_newnode_nop_1(calcifier_calc_node_t x, calcifier_calc_node_t y);
extern calcifier_calc_node_t calcifier_calc_newnode_setit_1(calcifier_calc_node_t x, calcifier_calc_node_t y);
extern calcifier_calc_node_t calcifier_calc_newnode_get_1(calcifier_calc_node_t x, calcifier_calc_node_t y);
extern calcifier_calc_node_t calcifier_calc_newnode_neg_1(calcifier_calc_node_t x, calcifier_calc_node_t y);
extern calcifier_calc_node_t calcifier_calc_newnode_not_1(calcifier_calc_node_t x, calcifier_calc_node_t y);
extern calcifier_calc_node_t calcifier_calc_newnode_abs_1(calcifier_calc_node_t x, calcifier_calc_node_t y);
extern calcifier_calc_node_t calcifier_calc_newnode_set_2(calcifier_calc_node_t x, calcifier_calc_node_t y);
extern calcifier_calc_node_t calcifier_calc_newnode_add_2(calcifier_calc_node_t x, calcifier_calc_node_t y);
extern calcifier_calc_node_t calcifier_calc_newnode_sub_2(calcifier_calc_node_t x, calcifier_calc_node_t y);
extern calcifier_calc_node_t calcifier_calc_newnode_mul_2(calcifier_calc_node_t x, calcifier_calc_node_t y);
extern calcifier_calc_node_t calcifier_calc_newnode_div_2(calcifier_calc_node_t x, calcifier_calc_node_t y);
extern calcifier_calc_node_t calcifier_calc_newnode_mod_2(calcifier_calc_node_t x, calcifier_calc_node_t y);
extern calcifier_calc_node_t calcifier_calc_newnode_equ_2(calcifier_calc_node_t x, calcifier_calc_node_t y);
extern calcifier_calc_node_t calcifier_calc_newnode_neq_2(calcifier_calc_node_t x, calcifier_calc_node_t y);
extern calcifier_calc_node_t calcifier_calc_newnode_lt_2(calcifier_calc_node_t x, calcifier_calc_node_t y);
extern calcifier_calc_node_t calcifier_calc_newnode_gt_2(calcifier_calc_node_t x, calcifier_calc_node_t y);
extern calcifier_calc_node_t calcifier_calc_newnode_then_2(calcifier_calc_node_t x, calcifier_calc_node_t y);
extern calcifier_calc_node_t calcifier_calc_newnode_pyif_2(calcifier_calc_node_t x, calcifier_calc_node_t y);
extern calcifier_calc_node_t calcifier_calc_newnode_else_2(calcifier_calc_node_t x, calcifier_calc_node_t y);
extern calcifier_calc_node_t calcifier_calc_newnode_while_2(calcifier_calc_node_t x, calcifier_calc_node_t y);
extern calcifier_calc_node_t calcifier_calc_newnode_do_2(calcifier_calc_node_t x, calcifier_calc_node_t y);


#endif
