#include "pbc/pbc.h"

typedef struct {
    element_t x;
    element_t y;
} VEC_STR;

VEC_STR* Encode(char* str, pairing_t pairing, int type) {
    int l = strlen(str);
    VEC_STR* Encode = (VEC_STR*)malloc(l * sizeof(VEC_STR));
    for (int i = 0; i < l; i++) {
        element_init_Zr(Encode[i].x, pairing);
        element_init_Zr(Encode[i].y, pairing);
        switch (type) {
            case 1:
                element_set_si(Encode[i].x, str[i] == '1' ? 0 : 1);
                element_set_si(Encode[i].y, 0);
                break;
            case 2:
                element_set_si(Encode[i].x, 0);
                if (str[i] == '0')
                    element_set_si(Encode[i].y, 0);
                else
                    element_random(Encode[i].y);
                break;
            case 3:
                element_set_si(Encode[i].x, 1);
                element_set_si(Encode[i].y, str[i] - '0');
                break;
            default:
                break;
        }

#ifdef DEBUG
        element_printf("%B, %B\n", Encode[i].x, Encode[i].y);
#endif
    }
    return Encode;
}

// char* mapping(char** set, char** keywords) {
//     int l = (int)( sizeof(set) / sizeof(set[0]);
//     for(int i = 0; i < l; i ++){
        
//     }
// }
