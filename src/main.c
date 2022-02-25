#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "DPVS.h"
#include "pbc/pbc.h"
#include "setup.h"

int main(int argc, char const* argv[]) {
    char param[1024];
    pairing_t pairing;
    size_t count = fread(param, 1, 1024, stdin);
    if (!count)
        return 1;
    pairing_init_set_buf(pairing, param, count);
    char tmp[16] = "1011001";
    Encode(tmp, pairing, 1);
    Encode(tmp, pairing, 2);
    Encode(tmp, pairing, 3);
    // keyGen(tmp, pairing);

    int** b = (int**)malloc(4 * sizeof(int*));
    for (int i = 0; i < 4; i++)
        b[i] = (int*)malloc(4 * sizeof(int));
    int a[4][4] = {
        {5, -2, 2, 7},
        {1, 0, 0, 3},
        {-3, 1, 5, 0},
        {3, -1, -9, 4}};

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            b[i][j] = a[i][j];
        }
    }

    inverse(b, 4);
    return 0;
}
