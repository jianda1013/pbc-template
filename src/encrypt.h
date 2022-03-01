#include <pbc.h>

#include "keygen.h"

typedef struct {
    element_t C_0;
    element_t C_1;
    element_t C_2;
    element_t* C_n;
    int* S_c;
} CT;

CT encrypt(Params pp, element_t msg, int* S, int n, pairing_t pairing) {
    CT ct;
    element_t s;
    element_init_Zr(s, pairing);
    element_random(s);
    element_init_GT(ct.C_0, pairing);
    element_init_G1(ct.C_1, pairing);
    element_init_G1(ct.C_2, pairing);
    element_pow_zn(ct.C_0, pp.Omega, s);
    element_mul(ct.C_0, ct.C_0, msg);
    element_pow_zn(ct.C_1, pp.G.g_1, s);
    element_pow_zn(ct.C_2, pp.T, s);
    ct.C_n = (element_t*)malloc(n * sizeof(element_t));
    for (int i = 0; i < n; i++) {
        element_init_G1(ct.C_n[i], pairing);
        if (S[i])
            element_pow_zn(ct.C_n[i], pp.X[i], s);
        else
            element_random(ct.C_n[i]);
    }
    ct.S_c = S;
#ifdef DEBUG
    element_printf("====================\n");
    element_printf("encrypt:\n");
    element_printf("    C_0:%B\n", ct.C_0);
    element_printf("    C_1:%B\n", ct.C_1);
    element_printf("    C_2:%B\n", ct.C_2);
    element_printf("    C_n:\n");
    for (int i = 0; i < n; i++)
        element_printf("        C[%d]:%B\n", i, ct.C_n[i]);
#endif
    return ct;
}
