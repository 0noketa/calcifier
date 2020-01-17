#include "calcifier_rtl.h"
#include "calcifier_calc.h"


int main(int argc, char *argv[]) {
    if (argc <= 1) return 1;

    char *expr = argv[1];
    calcifier_calc_node_t result;

    if (!calcifier_calc_tryParse(expr, strlen(expr), &result)) {
        fputs("error\n", stderr);
        return 1;
    }

    printf("%d\n", (int)calcifier_calc_eval(result));
    calcifier_calc_delnode(result);

    return 0;
}
