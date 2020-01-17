#include "calcifier_rtl.h"
#include "simplecalc.h"


int main(int argc, char *argv[]) {
    if (argc <= 1) return 1;

    char *expr = argv[1];
    simplecalc_node_t result;

    if (!simplecalc_tryParse(expr, strlen(expr), &result)) {
        fputs("error\n", stderr);
        return 1;
    }

    printf("%d\n", (int)result);
    simplecalc_delnode(result);

    return 0;
}
