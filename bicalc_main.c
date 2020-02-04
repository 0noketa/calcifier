#include "calcifier_rtl.h"
#include "bicalc.h"

// fgmp hasnt gmp_printf 
void printValue(MP_INT *x) {
    size_t size = mpz_sizeinbase(x, BICALC_BASE);
    char *buf = malloc(size + 2); // + sign + eos

    if (buf) {
        mpz_get_str(buf, BICALC_BASE, x);

        for (char *p = buf; *p; ++p) *p = toupper(*p);

        puts(buf);

        free(buf);
    }
}


int main(int argc, char *argv[]) {
    if (argc <= 1) return 1;

    char *expr = argv[1];
    bicalc_node_t result;

    if (!bicalc_tryParse(expr, strlen(expr), &result)) {
        fputs("error\n", stderr);

        return 1;
    }

    printValue(result);
    bicalc_delnode(result);

    return 0;
}
