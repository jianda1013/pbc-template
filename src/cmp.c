#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "pbc/pbc.h"

#define AUTHORITY_NUMBER 25
#define MATRIX 1

typedef struct {
    element_t g1;
    element_t g2;
    element_t A[MATRIX + 1][MATRIX];
    element_t U[MATRIX + 1][MATRIX + 1];
    element_t g_A[MATRIX + 1][MATRIX];
    element_t g_UA[MATRIX + 1][MATRIX + 1];
} PP;

typedef struct {
    element_t W[MATRIX + 1][MATRIX + 1];
    element_t alpha[MATRIX + 1];
    element_t sigma;
} SK;

typedef struct {
    element_t g1[MATRIX + 1][MATRIX];
    element_t e[MATRIX];
    element_t y;
} PK;

typedef struct {
    SK secret_key;
    PK public_key;
} AUTH;

typedef struct {
    element_t C0[MATRIX + 1];
    element_t Ci[AUTHORITY_NUMBER][MATRIX + 1];
    element_t C_prime;
} CTEXT;

typedef struct {
    element_t K[MATRIX + 1];
    element_t H[MATRIX + 1];
} KEY;

pairing_t pairing;

PP Setup() {
    PP public_parameter;
    element_init_G1(public_parameter.g1, pairing);
    element_init_G1(public_parameter.g2, pairing);
    element_random(public_parameter.g1);
    element_random(public_parameter.g2);

    element_t A[MATRIX + 1][MATRIX], U[MATRIX + 1][MATRIX + 1];

    for (int i = 0; i <= MATRIX; i++) {
        for (int j = 0; j <= MATRIX; j++) {
            if (j != MATRIX) {
                element_init_Zr(public_parameter.A[i][j], pairing);
                element_random(public_parameter.A[i][j]);
                element_init_G1(public_parameter.g_A[i][j], pairing);
                element_pow_zn(public_parameter.g_A[i][j], public_parameter.g1, public_parameter.A[i][j]);
            }
            element_init_Zr(public_parameter.U[i][j], pairing);
            element_random(public_parameter.U[i][j]);
        }
    }

    for (int i = 0; i <= MATRIX; i++) {
        for (int j = 0; j < MATRIX; j++) {
            element_t pow;
            element_init_Zr(pow, pairing);
            element_mul(pow, public_parameter.U[0][i], public_parameter.A[0][j]);
            for (int k = 1; k <= MATRIX; k++) {
                element_t adder;
                element_init_Zr(adder, pairing);
                element_mul(adder, public_parameter.U[k][i], public_parameter.A[k][j]);
                element_add(pow, pow, adder);
            }
            element_init_G1(public_parameter.g_UA[i][j], pairing);
            element_pow_zn(public_parameter.g_UA[i][j], public_parameter.g1, pow);
        }
    }

    return public_parameter;
}

AUTH AuthSetup(PP public_parameter) {
    AUTH key;
    for (int i = 0; i <= MATRIX; i++) {
        for (int j = 0; j <= MATRIX; j++) {
            element_init_Zr(key.secret_key.W[i][j], pairing);
            element_random(key.secret_key.W[i][j]);
        }
        element_init_Zr(key.secret_key.alpha[i], pairing);
        element_random(key.secret_key.alpha[i]);
    }
    element_init_Zr(key.secret_key.sigma, pairing);
    element_random(key.secret_key.sigma);

    for (int i = 0; i <= MATRIX; i++) {
        for (int j = 0; j < MATRIX; j++) {
            element_t pow;
            element_init_Zr(pow, pairing);
            element_mul(pow, key.secret_key.W[0][i], public_parameter.A[0][j]);
            for (int k = 1; k <= MATRIX; k++) {
                element_t adder;
                element_init_Zr(adder, pairing);
                element_mul(adder, key.secret_key.W[k][i], public_parameter.A[k][j]);
                element_add(pow, pow, adder);
            }
            element_init_G1(key.public_key.g1[i][j], pairing);
            element_pow_zn(key.public_key.g1[i][j], public_parameter.g1, pow);
        }
    }

    element_t e;
    element_init_GT(e, pairing);
    element_pairing(e, public_parameter.g1, public_parameter.g2);

    for (int i = 0; i < MATRIX; i++) {
        element_t pow;
        element_init_Zr(pow, pairing);
        element_mul(pow, key.secret_key.alpha[0], public_parameter.A[0][i]);
        for (int j = 1; j <= MATRIX; j++) {
            element_t adder;
            element_init_Zr(adder, pairing);
            element_mul(adder, key.secret_key.alpha[i], public_parameter.A[j][i]);
            element_add(pow, pow, adder);
        }
        element_init_GT(key.public_key.e[i], pairing);
        element_pow_zn(key.public_key.e[i], e, pow);
    }

    element_init_G1(key.public_key.y, pairing);
    element_pow_zn(key.public_key.y, public_parameter.g2, key.secret_key.sigma);

    return key;
}

CTEXT encrypt(PP pp, PK **pk, element_t *x) {
    CTEXT ciphertext;
    element_t s[MATRIX];
    for (int i = 0; i < MATRIX; i++) {
        element_init_Zr(s[i], pairing);
        element_random(s[i]);
    }

    element_t As[MATRIX];
    for (int i = 0; i <= MATRIX; i++) {
        element_init_Zr(As[i], pairing);
        element_mul(As[i], pp.A[0][i], s[0]);
        for (int j = 0; j < MATRIX; j++) {
            element_t adder;
            element_init_Zr(adder, pairing);
            element_mul(adder, pp.A[j][i], s[j]);
            element_add(As[i], As[i], adder);
        }
        element_init_G1(ciphertext.C0[i], pairing);
        element_pow_zn(ciphertext.C0[i], pp.g1, As[i]);
    }

    for (int i = 0; i < AUTHORITY_NUMBER; i++) {
        element_t left[MATRIX + 1][MATRIX];
        for (int j = 0; j <= MATRIX; j++) {
            for (int k = 0; k < MATRIX; k++) {
                element_init_G1(left[j][k], pairing);
                element_set(left[j][k], pp.g_UA[j][k]);
                element_pow_zn(left[j][k], left[j][k], x[i]);
                element_mul(left[j][k], left[j][k], pk[i]->g1[j][k]);
            }
        }
        for (int j = 0; j <= MATRIX; j++) {
            element_init_G1(ciphertext.Ci[i][j], pairing);
            element_mul(ciphertext.Ci[i][j], left[j][0], s[0]);
            for (int k = 0; k < MATRIX; k++) {
                element_t adder;
                element_init_G1(adder, pairing);
                element_mul(adder, left[j][k], s[k]);
                element_add(ciphertext.Ci[i][j], ciphertext.Ci[i][j], adder);
            }
        }
    }

    element_init_Zr(ciphertext.C_prime, pairing);
    for (int i = 0; i < AUTHORITY_NUMBER; i++) {
        element_t each;
        element_init_Zr(each, pairing);
        element_mul(each, pk[i]->e[0], s[0]);
        for (int j = 0; j < MATRIX; j++) {
            element_t adder;
            element_init_Zr(adder, pairing);
            element_mul(adder, pk[i]->e[j], s[j]);
            element_add(each, each, adder);
        }
        if (i == 0)
            element_set(ciphertext.C_prime, each);
        else
            element_add(ciphertext.C_prime, ciphertext.C_prime, each);
    }

    return ciphertext;
}

KEY KeyGen(PP pp, PK **pk, SK sk, char **GID, int *v, int index) {
    KEY key;
    element_t Y[AUTHORITY_NUMBER];
    for (int i = 0; i < AUTHORITY_NUMBER; i++) {
        element_init_G1(Y[i], pairing);
        element_pow_zn(Y[i], pk[i]->y, sk.sigma);
    }
    element_t u[MATRIX + 1];
    for (int i = 0; i <= MATRIX; i++) {
        element_init_G1(u[i], pairing);
        element_init_G1(key.H[i], pairing);
        element_set0(u[i]);
        element_from_hash(key.H[i], GID[i], strlen(GID[i]));
        for (int j = 0; j < AUTHORITY_NUMBER; j++) {
            element_t hash;
            element_init_G1(hash, pairing);
            element_add(hash, u[i], Y[j]);
            if (j < index)
                element_add(u[i], u[i], hash);
            else if (j > index)
                element_sub(u[i], u[i], hash);
        }
    }
    element_t pow[MATRIX + 1];
    for (int i = 0; i <= MATRIX; i++) {
        element_t W_h;
        element_init_Zr(pow[i], pairing);
        element_init_Zr(W_h, pairing);
        element_set(pow[i], sk.alpha[i]);
        element_set0(W_h);
        for (int j = 0; j <= MATRIX; j++) {
            element_t adder;
            element_init_Zr(adder, pairing);
            element_mul(adder, sk.W[i][j], key.H[j]);
            element_add(W_h, W_h, adder);
        }
        element_mul_si(W_h, W_h, v[index]);
        element_sub(pow[i], pow[i], W_h);
        element_add(pow[i], pow[i], u[i]);
        element_init_G1(key.K[i], pairing);
        element_pow_zn(key.K[i], pp.g2, pow[i]);
    }
    return key;
}

void decrypt(PP pp, KEY *key, CTEXT C, int *v) {
    element_t e_left;
    element_init_GT(e_left, pairing);
    element_pairing(e_left, pp.g1, pp.g2);
    element_t pow;
    element_init_G1(pow, pairing);
    element_set0(pow);
    for (int i = 0; i <= MATRIX; i++) {
        element_t mul;
        element_init_G1(mul, pairing);
        element_set1(mul);
        for (int j = 0; j < AUTHORITY_NUMBER; j++)
            element_mul(mul, mul, key[j].K[i]);
        element_mul(mul, mul, C.C0[i]);
        element_add(pow, pow, mul);
    }
    element_pow_zn(e_left, e_left, pow);

    element_t e_right;
    element_init_GT(e_right, pairing);
    element_pairing(e_right, pp.g1, pp.g2);
    element_t pow2;
    element_init_G1(pow2, pairing);
    element_set0(pow2);
    for (int i = 0; i <= MATRIX; i++) {
        element_t mul;
        element_init_G1(mul, pairing);
        element_set1(mul);
        for (int j = 0; j < AUTHORITY_NUMBER; j++) {
            element_mul(mul, mul, C.Ci[j][i]);
        }
        element_mul(mul, mul, key[i].H[i]);
        element_add(pow2, pow2, mul);
    }
    element_pow_zn(e_right, e_right, pow2);
    element_mul(e_right, e_left, e_right);
}

int main(int argc, char const *argv[]) {
    clock_t now, end;
    char param[1024];
    size_t count = fread(param, 1, 1024, stdin);
    if (!count)
        return 1;
    pairing_init_set_buf(pairing, param, count);
    printf("{\"No\": %d, \n", AUTHORITY_NUMBER);
    now = clock();
    PP setup = Setup();
    end = clock();
    printf("\"SetUp\": %f,\n", (double)(end - now) / CLOCKS_PER_SEC * 1000);

    AUTH Authorize[AUTHORITY_NUMBER];
    for (int i = 0; i < AUTHORITY_NUMBER; i++) {
        if (i == 0) {
            now = clock();
            Authorize[i] = AuthSetup(setup);
            end = clock();
            printf("\"AuthSetup\": %f,\n", (double)(end - now) / CLOCKS_PER_SEC * 1000);
        } else
            Authorize[i] = AuthSetup(setup);
    }

    int X[] = {5, 10, 13, 16, 17,
               3, 5, 10, 11, 4,
               16, 14, 44, 2, 4,
               6, 4, 12, 33, 41,
               57, 25, 26, 13, 45,
               23, 14, 17, 3, 4};
    element_t x[AUTHORITY_NUMBER];
    for (int i = 0; i < AUTHORITY_NUMBER; i++) {
        element_init_Zr(x[i], pairing);
        element_set_si(x[i], X[i]);
    }
    PK *public_key_arr[AUTHORITY_NUMBER];
    for (int i = 0; i < AUTHORITY_NUMBER; i++)
        public_key_arr[i] = &Authorize[i].public_key;

    now = clock();
    CTEXT cypher_text = encrypt(setup, public_key_arr, x);
    end = clock();
    printf("\"Encrypt\": %f,\n", (double)(end - now) / CLOCKS_PER_SEC * 1000);
    char *text[AUTHORITY_NUMBER + 1];
    text[0] = "test0";
    text[1] = "test1";
    text[2] = "test2";
    text[3] = "test3";
    text[4] = "test4";
    text[5] = "test5";
    text[6] = "test6";
    text[7] = "test7";
    text[8] = "test8";
    text[9] = "test9";
    text[10] = "test10";

    KEY key[AUTHORITY_NUMBER];
    for (int i = 0; i < AUTHORITY_NUMBER; i++) {
        if (i == 0) {
            now = clock();
            key[i] = KeyGen(setup, public_key_arr, Authorize[i].secret_key, text, X, i);
            end = clock();
            printf("\"KeyGen\": %f,\n", (double)(end - now) / CLOCKS_PER_SEC * 1000);
        } else
            key[i] = KeyGen(setup, public_key_arr, Authorize[i].secret_key, text, X, i);
    }
    now = clock();
    decrypt(setup, key, cypher_text, X);
    end = clock();
    printf("\"Decrypt\": %f},\n", (double)(end - now) / CLOCKS_PER_SEC * 1000);
    return 0;
}
// clock_t now = clock();
// clock_t end = clock();
// printf("%f", (double)(end - now) / CLOCKS_PER_SEC);