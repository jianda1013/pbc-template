#include <stdio.h>
#include <stdlib.h>

#include "pbc/pbc.h"

#define AUTHORITY_NUMBER 10
#define LENGTH 10

typedef struct {
    element_t* alpha;
    element_t Zi;
} PK;

int main(int argc, char const* argv[]) {
    element_t alpha[AUTHORITY_NUMBER][LENGTH], g_alpha[AUTHORITY_NUMBER][LENGTH];  // init α[i][l], init g^α[i][l]                                                            //
    element_t alpha_i[AUTHORITY_NUMBER];                                           // init αi
    element_t Z_i[AUTHORITY_NUMBER];                                               // init Zi
    element_t g, test;                                                             // generator g ← G
    pairing_pp_t pp;                                                               //
    pairing_t pairing;                                                             //
    PK public_key[10];                                                             //
                                                                                   //                                                                                   //
    char param[1024];
    size_t count = fread(param, 1, 1024, stdin);
    if (!count)
        return 5;
    pairing_init_set_buf(pairing, param, count);
    element_init_G1(g, pairing);  //
    element_random(g);            // init g

    //
    for (int i = 0; i < AUTHORITY_NUMBER; i++) {  //
        element_init_G1(alpha_i[i], pairing);
        element_random(alpha_i[i]);  // Choose αi
        for (int j = 0; j < LENGTH; j++) {
            element_init_G1(alpha[i][j], pairing);  //
            element_random(alpha[i][j]);            // chose α(0,i) to α(l,i)
        }
        public_key[i].alpha = alpha[i];
        element_init_G1(public_key[i].Zi, pairing);
        pairing_pp_init(pp, g, pairing);
        element_init_G1(test, pairing);
        // pairing_pp_apply(test, g, pp);
        pairing_pp_clear(pp);
    }

    for (int i = 0; i < AUTHORITY_NUMBER; i++) {
        for (int j = 0; j < LENGTH; j++) {
            element_printf("%B ", public_key[i].alpha[j]);
        }
        element_printf(" \n ==============\n");
    }

    return 0;
}
