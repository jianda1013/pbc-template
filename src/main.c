#include "decrypt.h"
// #include "setup.h"
int main(int argc, char const* argv[]) {
    // ************************************* //
    // set the pairing definition            //

    char param[1024];
    pairing_t pairing;
    size_t count = fread(param, 1, 1024, stdin);
    if (!count)
        return 1;
    pairing_init_set_buf(pairing, param, count);

    // ************************************* //

    SETUP set = setup(5, pairing);

    // ************************************* //
    // set given set S_k

    int* s = (int*)malloc(5 * sizeof(int));
    int s_prime[5] = {0, 1, 1, 1, 1};
    for (int i = 0; i < 5; i++) {
        s[i] = s_prime[i];
    }

    // ************************************* //
    SK key = keygen(set.pp, set.msk, s, 5, pairing);

    // ************************************* //
    // set M = msg, set S_c

    element_t msg;
    element_init_GT(msg, pairing);
    element_from_hash(msg, "test", 4);

    int* t = (int*)malloc(5 * sizeof(int));
    int t_prime[5] = {0, 1, 1, 1, 0};
    for (int i = 0; i < 5; i++) {
        t[i] = t_prime[i];
    }

    // ************************************* //

    CT cypherT = encrypt(set.pp, msg, t, 5, pairing);
    DC d_msg = decrypt(set.pp, cypherT, key, 5, pairing);

    element_printf("%d\n", element_cmp(msg, d_msg.msg));

    return 0;
}
