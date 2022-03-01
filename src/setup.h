#include <pbc.h>

typedef struct {
    element_t g_1;
    element_t g_2;
    element_t g_t;
} G_notation;

typedef struct {
    G_notation G;
    element_t* X;
    element_t T;
    element_t Omega;
} Params;

typedef struct {
    element_t* x;
    element_t w;
    element_t t;
} MSK;

typedef struct {
    Params pp;
    MSK msk;
} SETUP;

SETUP setup(int n, pairing_t pairing) {
    SETUP setup;
    element_init_G1(setup.pp.G.g_1, pairing);
    element_init_G2(setup.pp.G.g_2, pairing);
    element_init_GT(setup.pp.G.g_t, pairing);
    element_random(setup.pp.G.g_1);
    element_random(setup.pp.G.g_2);
    element_pairing(setup.pp.G.g_t, setup.pp.G.g_1, setup.pp.G.g_2);
    setup.msk.x = (element_t*)malloc(n * sizeof(element_t));
    setup.pp.X = (element_t*)malloc(n * sizeof(element_t));
    for (int i = 0; i < n; i++) {
        element_init_Zr(setup.msk.x[i], pairing);
        element_random(setup.msk.x[i]);
        element_init_G1(setup.pp.X[i], pairing);
        element_pow_zn(setup.pp.X[i], setup.pp.G.g_1, setup.msk.x[i]);
    }
    element_init_Zr(setup.msk.w, pairing);
    element_init_Zr(setup.msk.t, pairing);
    element_init_GT(setup.pp.Omega, pairing);
    element_init_G1(setup.pp.T, pairing);
    element_random(setup.msk.w);
    element_random(setup.msk.t);
    element_pow_zn(setup.pp.Omega, setup.pp.G.g_t, setup.msk.w);
    element_pow_zn(setup.pp.T, setup.pp.G.g_1, setup.msk.t);
#ifdef DEBUG
    element_printf("====================\n");
    element_printf("param:\n");
    element_printf("    G:\n");
    element_printf("        g1:%B\n", setup.pp.G.g_1);
    element_printf("        g2:%B\n", setup.pp.G.g_2);
    element_printf("        gt:%B\n", setup.pp.G.g_t);
    element_printf("    T:%B\n", setup.pp.T);
    element_printf("    Ω:%B\n", setup.pp.Omega);
    element_printf("    X:\n");
    for (int i = 0; i < n; i++)
        element_printf("        X[%d]:%B\n", i, setup.pp.X[i]);
    element_printf("msk:\n");
    element_printf("    t:%B\n", setup.msk.t);
    element_printf("    w:%B\n", setup.msk.w);
    element_printf("    x:\n");
    for (int i = 0; i < n; i++)
        element_printf("        x[%d]:%B\n", i, setup.msk.x[i]);
#endif
    return setup;
}