#include <pbc.h>

#include "setup.h"

typedef struct {
    element_t K_1;
    element_t K_2;
    element_t K_3;
    int* S_k;
} SK;

SK keygen(Params pp, MSK msk, int* S, int n, pairing_t pairing) {
    SK sk;
    element_t u, v;
    element_init_Zr(u, pairing);
    element_init_Zr(v, pairing);
    element_random(u);
    element_random(v);
    element_t X_k;
    element_init_Zr(X_k, pairing);
    element_set0(X_k);
    for (int i = 0; i < n; i++)
        if (S[i])
            element_add(X_k, X_k, msk.x[i]);
    element_t tmp;
    element_init_Zr(tmp, pairing);
    element_set(tmp, msk.t);
    element_mul(tmp, tmp, v);
    element_mul(X_k, X_k, u);
    element_add(X_k, X_k, msk.w);
    element_add(X_k, X_k, tmp);
    element_neg(X_k, X_k);
    element_init_G2(sk.K_1, pairing);
    element_init_G2(sk.K_2, pairing);
    element_init_G2(sk.K_3, pairing);
    element_pow_zn(sk.K_1, pp.G.g_2, X_k);
    element_pow_zn(sk.K_2, pp.G.g_2, v);
    element_pow_zn(sk.K_3, pp.G.g_2, u);
    sk.S_k = S;
#ifdef DEBUG
    element_printf("====================\n");
    element_printf("keygen:\n");
    element_printf("    K_1:%B\n", sk.K_1);
    element_printf("    K_2:%B\n", sk.K_2);
    element_printf("    K_3:%B\n", sk.K_3);
#endif
    return sk;
}
