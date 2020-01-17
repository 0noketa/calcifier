#include "calcifier_rtl.h"
#include "bicalc.h"

// fgmp hasnt gmp_printf 
void printHex(MP_INT *x)
{
    size_t size = mpz_size(x);
    char *buf = malloc(size * 16 + 1);

    if (buf)
    {
        mpz_get_str(buf, 16, x);

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

    printHex(result);
    bicalc_delnode(result);

    return 0;
}
