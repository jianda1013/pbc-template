#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pbc/pbc.h"

#define AUTHORITY_NUMBER 10
#define LENGTH 10

typedef struct {
    element_t g_alpha[LENGTH + 1];  //  G1[]
    element_t Zi;                   //  GT
} PK;                               //
                                    //
typedef struct {                    //
    element_t alpha[LENGTH + 1];    //  Zr[]
    element_t g_alpha;              //  G1
} MSK;                              //
                                    //
typedef struct {                    //
    PK public_key;                  //
    MSK master_secret_key;          //
} AUTH;                             //
                                    //
typedef struct {                    //
    element_t D0;                   //  G1
    element_t D1;                   //  G1
    element_t K[LENGTH + 1];        //  G1[]
} SK;                               //
                                    //
typedef struct {                    //
    element_t E0;                   //  GT
    element_t E1;                   //  G1
    element_t E2;                   //  G1
} CTEXT;

AUTH authorize[AUTHORITY_NUMBER];
SK user_secret_key[AUTHORITY_NUMBER];

pairing_t pairing;
char message[256];

void AuthSetup(element_t g, int i) {
    element_t e;
    element_init_GT(e, pairing);
    element_pairing(e, g, g);

    element_t alpha;
    element_init_Zr(alpha, pairing);
    element_random(alpha);

    for (int j = 0; j <= LENGTH; j++) {
        element_init_Zr(authorize[i].master_secret_key.alpha[j], pairing);
        element_init_G1(authorize[i].public_key.g_alpha[j], pairing);
        element_random(authorize[i].master_secret_key.alpha[j]);
        element_pow_zn(authorize[i].public_key.g_alpha[j], g, authorize[i].master_secret_key.alpha[j]);
    }

    element_init_GT(authorize[i].public_key.Zi, pairing);
    element_init_G1(authorize[i].master_secret_key.g_alpha, pairing);
    element_pow_zn(authorize[i].public_key.Zi, e, alpha);
    element_pow_zn(authorize[i].master_secret_key.g_alpha, g, alpha);

    element_clear(alpha);
    element_clear(e);
    return;
}

SK KeyGen(element_t g, MSK master_key, char* GID, int* vector_x) {
    SK secret_key;
    element_init_G1(secret_key.D0, pairing);
    element_init_G1(secret_key.D1, pairing);
    element_from_hash(secret_key.D0, GID, strlen(GID));
    element_pow_zn(secret_key.D1, secret_key.D0, master_key.alpha[0]);
    element_mul(secret_key.D1, master_key.g_alpha, secret_key.D1);

    element_t xi;
    element_init_Zr(xi, pairing);
    element_set_si(xi, vector_x[0]);
    for (int j = 2; j <= LENGTH; j++) {
        element_t xj, front, back;
        element_init_Zr(xj, pairing);
        element_init_Zr(front, pairing);
        element_init_G1(back, pairing);

        element_set_si(xj, vector_x[j - 1]);
        element_neg(front, master_key.alpha[1]);
        element_mul(front, front, xj);
        element_div(front, front, xi);
        element_pow_zn(front, secret_key.D0, front);
        element_pow_zn(back, secret_key.D0, master_key.alpha[j]);

        element_init_G1(secret_key.K[j], pairing);
        element_mul(secret_key.K[j], front, back);

        element_clear(xj);
        element_clear(front);
        element_clear(back);
    }
    element_clear(xi);
    return secret_key;
}

CTEXT encrypt(element_t g, PK** pk, element_t msg, int* vector_y) {
    element_t s;
    element_init_Zr(s, pairing);
    element_random(s);
    CTEXT ciphertext;

    element_init_GT(ciphertext.E0, pairing);
    element_init_G1(ciphertext.E1, pairing);
    element_init_G1(ciphertext.E2, pairing);
    element_set(ciphertext.E0, pk[0]->Zi);

    for (int i = 1; i < AUTHORITY_NUMBER; i++)
        element_mul(ciphertext.E0, ciphertext.E0, pk[i]->Zi);
    element_pow_zn(ciphertext.E0, ciphertext.E0, s);
    element_mul(ciphertext.E0, ciphertext.E0, msg);

    for (int i = 0; i <= LENGTH; i++) {
        element_t params;
        element_init_G1(params, pairing);
        element_set(params, pk[0]->g_alpha[i]);
        for (int j = 1; j < AUTHORITY_NUMBER; j++)
            element_mul(params, params, pk[j]->g_alpha[i]);
        if (i != 0) {
            element_t y;
            element_init_Zr(y, pairing);
            element_set_si(y, vector_y[i]);
            element_pow_zn(params, params, y);
            element_mul(ciphertext.E1, ciphertext.E1, params);
            element_clear(y);
        } else
            element_set(ciphertext.E1, params);
        element_clear(params);
    }
    element_pow_zn(ciphertext.E1, ciphertext.E1, s);
    element_pow_zn(ciphertext.E2, g, s);

    element_clear(s);
    return ciphertext;
}

int decrypt(SK* sk, CTEXT C, int* vector_y) {
    element_t upper_left, upper;
    element_init_G1(upper_left, pairing);
    element_init_GT(upper, pairing);
    element_set(upper_left, sk[0].D1);
    for (int i = 1; i < AUTHORITY_NUMBER; i++)
        element_mul(upper_left, upper_left, sk[i].D1);
    for (int i = 2; i < LENGTH; i++) {
        element_t base;
        element_init_G1(base, pairing);
        element_printf("%d, %B\n", i, sk[i].D0);
        // element_set(base, sk[i].K[2]);
        // for (int j = 1; j < AUTHORITY_NUMBER; j++)
        //     element_mul(base, base, sk[i].K[j]);
        // element_t y;
        // element_init_Zr(y, pairing);
        // element_set_si(y, vector_y[i - 1]);
        // element_pow_zn(base, base, y);
        // element_mul(upper_left, upper_left, base);
        // element_clear(base);
        // element_clear(y);
    }
    element_printf("%B", upper_left);
    // element_pairing(upper, )
}

int main(int argc, char const* argv[]) {
    char param[1024];
    size_t count = fread(param, 1, 1024, stdin);
    if (!count)
        return 1;
    pairing_init_set_buf(pairing, param, count);

    element_t g;
    element_init_G1(g, pairing);  //
    element_random(g);            // init g

    for (int i = 0; i < AUTHORITY_NUMBER; i++)
        AuthSetup(g, i);

    int vector[LENGTH];
    int X[] = {5, 10, 13, 16, 17, 3, 5, 10, 11, 4};
    for (int i = 0; i < AUTHORITY_NUMBER; i++)
        user_secret_key[i] = KeyGen(g, authorize[i].master_secret_key, "test", X);

    PK* public_key_arr[AUTHORITY_NUMBER];
    for (int i = 0; i < AUTHORITY_NUMBER; i++)
        public_key_arr[i] = &authorize[i].public_key;
    element_t rdM;
    element_init_GT(rdM, pairing);
    element_random(rdM);
    CTEXT cypher_text = encrypt(g, public_key_arr, rdM, X);

    decrypt(user_secret_key, cypher_text, X);

    return 0;
}
