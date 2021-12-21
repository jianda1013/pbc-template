#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pbc/pbc.h"

#define AUTHORITY_NUMBER 10
#define LENGTH 10

typedef struct {
    element_t g_alpha[LENGTH];
    element_t Zi;
} PK;

typedef struct {
    element_t alpha[LENGTH];
    element_t g_alpha;
} MSK;

typedef struct {
    PK public_key;
    MSK master_secret_key;
} AUTH;

typedef struct {
    element_t D0;
    element_t D1;
    element_t* K;
} SK;

typedef struct {
    element_t E0;
    element_t E1;
    element_t E2;
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
    element_init_G1(alpha, pairing);
    element_random(alpha);

    for (int j = 0; j < LENGTH; j++) {
        element_init_G1(authorize[i].master_secret_key.alpha[j], pairing);
        element_init_G1(authorize[i].public_key.g_alpha[j], pairing);
        element_random(authorize[i].master_secret_key.alpha[j]);
        element_pow_zn(authorize[i].public_key.g_alpha[j], g, authorize[i].master_secret_key.alpha[j]);
    }

    element_init_GT(authorize[i].public_key.Zi, pairing);
    element_init_GT(authorize[i].master_secret_key.g_alpha, pairing);
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
    element_init_G1(xi, pairing);
    element_set_si(xi, vector_x[0]);
    for (int i = 1; i < LENGTH; i++) {
        element_t xj, front, back;
        element_init_G1(xj, pairing);
        element_init_G1(front, pairing);
        element_init_G1(back, pairing);

        element_set_si(xj, vector_x[i]);
        element_neg(front, master_key.alpha[1]);
        element_mul(front, front, xj);
        element_div(front, front, xi);
        element_pow_zn(front, secret_key.D0, front);
        element_pow_zn(back, secret_key.D0, master_key.alpha[i]);

        element_init_G1(secret_key.K[i - 1], pairing);
        element_mul(secret_key.K[i - 1], front, back);

        element_clear(xj);
        element_clear(front);
        element_clear(back);
    }
    element_clear(xi);
    return secret_key;
}

CTEXT encrypt(element_t g, PK* pk, char* msg, element_t* vector_Y) {
    element_t s;
    element_init_G1(s, pairing);
    element_random(s);
    CTEXT ciphertext;

    element_init_G1(ciphertext.E0, pairing);
    element_set(ciphertext.E0, pk[0].Zi);

    for (int i = 1; i < AUTHORITY_NUMBER; i++) {
        element_mul(ciphertext.E0, ciphertext.E0, pk[i].Zi);
    }
    element_pow_zn(ciphertext.E0, ciphertext.E0, s);
    
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
    int a[] = {5, 10, 13, 16, 17, 3, 5, 10, 11, 4};
    for (int i = 0; i < AUTHORITY_NUMBER; i++)
        KeyGen(g, authorize[i].master_secret_key, "test", a);

    return 0;
}
