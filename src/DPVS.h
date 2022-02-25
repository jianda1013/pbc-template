#include "pbc/pbc.h"

typedef struct {
    element_t** r;
    element_t** r_star;
} DPVS;

element_t** subArray(element_t** arr, int p, int q, int size, pairing_t pairing) {
    element_t** sub_arr = (element_t**)malloc((size - 1) * sizeof(element_t*));
    int col = 0, row = 0;
    for (int i = 0; i < size; i++) {
        row = 0;
        if (i != p) {
            sub_arr[col] = (element_t*)malloc((size - 1) * sizeof(element_t));
            for (int j = 0; j < size; j++) {
                if (j != q) {
                    element_init_Zr(sub_arr[col][row], pairing);
                    element_set(sub_arr[col][row++], arr[i][j]);
                }
            }
            col++;
        }
    }

#ifdef DEBUG
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - 1; j++) {
            element_printf("%B ", sub_arr[i][j]);
        }
        printf("\n");
    }
#endif
    return sub_arr;
}

element_t* disjoint(element_t** arr, pairing_t pairing, int l) {
    if (l == 1)
        return arr[0];
    element_t* result;
    element_init_Zr(*result, pairing);
    element_set_si(*result, 0);
    element_t** sub_arr = subArray(arr, 0, 0, l, pairing);

    for (int i = 0; i < l; i++) {
        for (int j = 0; j < l; j++) {
            element_t** sub_arr = subArray(arr, i, j, l, pairing);
            if ((i + j) % 2 == 0)
                element_add(*result, *result, *disjoint(sub_arr, pairing, l - 1));
            else
                element_sub(*result, *result, *disjoint(sub_arr, pairing, l - 1));
        }
    }
#ifdef DEBUG
    element_printf("%B ", *result);
#endif

    return result;
}

DPVS getB(int l, pairing_t pairing) {
    DPVS Bs;
    Bs.r = (element_t**)malloc(l * sizeof(element_t*));
    Bs.r_star = (element_t**)malloc(l * sizeof(element_t*));
    for (int i = 0; i < l; i++) {
        Bs.r[i] = (element_t*)malloc(l * sizeof(element_t));
        for (int j = 0; j < l; j++) {
            element_init_Zr(Bs.r[i][j], pairing);
            element_random(Bs.r[i][j]);
#ifdef DEBUG
            element_printf("%B ", Bs.r[i][j]);
#endif
        }
#ifdef DEBUG
        element_printf("\n");
#endif
    }
    disjoint(Bs.r, pairing, l);
    return Bs;
}

void keyGen(char* str, pairing_t pairing) {
    int l = strlen(str);
    DPVS B = getB(l, pairing);
}

void getCofactor(int** arr, int** temp, int row_now, int col_now, int l) {
    int i = 0, j = 0;
    for (int row = 0; row < l; row++) {
        for (int col = 0; col < l; col++) {
            if (row != row_now && col != col_now) {
                temp[i][j++] = arr[row_now][col_now];
                if (j == l - 1) {
                    j = 0;
                    i++;
                }
            }
        }
    }
}

int determinant(int** A, int l, int size) {
    int determin = 0;
    if (l == 1){
        printf("%d OUO\n", A[0][0]);
        return A[0][0];
    }

    int** temp = (int**)malloc(size * sizeof(int*));
    for (int i = 0; i < size; i++)
        temp[i] = (int*)malloc(size * sizeof(int));

    for (int i = 0; i < l; i++) {
        getCofactor(A, temp, 0, i, l);
        determin += (i % 2 == 0 ? 1 : -1) * A[0][i] * determinant(temp, l - 1, size);
    }
    printf("%d\n", determin);
    return determin;
}

// Function to get adjoint of A[N][N] in adj[N][N].
void adjoint(int** A, int** adj, int size) {
    if (size == 1) {
        adj[0][0] = 1;
        return;
    }

    int** temp = (int**)malloc(size * sizeof(int*));
    for (int i = 0; i < size; i++)
        temp[i] = (int*)malloc(size * sizeof(int));

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            getCofactor(A, temp, i, j, size);
            adj[j][i] = (((i + j) % 2 == 0) ? 1 : -1) * (determinant(temp, size - 1, size));
        }
    }
}

int** inverse(int** A, int size) {
    int** adj = (int**)malloc(size * sizeof(int*));
    for (int i = 0; i < size; i++)
        adj[i] = (int*)malloc(size * sizeof(int));
    adjoint(A, adj, size);

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            printf("%3d ", adj[i][j]);
        }
        printf("\n");
    }

    return adj;
}