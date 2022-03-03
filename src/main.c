#include <pbc.h>

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

    return 0;
}
