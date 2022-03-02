#include <pbc.h>

#include "encrypt.h"

typedef struct {
    element_t msg;
} DC;

DC decrypt(Params pp, CT ct, SK sk, int n, pairing_t pairing) {
    DC re;
    element_t C_3;
    element_init_G1(C_3, pairing);
    element_set1(C_3);
    for (int i = 0; i < n; i++) {
        if (sk.S_k[i])
            element_mul(C_3, C_3, ct.C_n[i]);
    }
    element_init_GT(re.msg, pairing);
    element_set1(re.msg);
    element_t tmp;
    element_init_GT(tmp, pairing);
    element_pairing(tmp, ct.C_1, sk.K_1);
    element_mul(re.msg, re.msg, tmp);
    element_pairing(tmp, ct.C_2, sk.K_2);
    element_mul(re.msg, re.msg, tmp);
    element_pairing(tmp, C_3, sk.K_3);
    element_mul(re.msg, re.msg, tmp);
    element_mul(re.msg, ct.C_0, re.msg);
    return re;
}