#ifndef PBC_H
#define PBC_H

#include <pbc.h>

element_t newElement(pairing_t pairing, char* type) {
    element_t Zr;
    if (strcmp(type, "G1") == 0)
        element_init_G1(Zr, pairing);
    else if (strcmp(type, "G2") == 0)
        element_init_G2(Zr, pairing);
    else if (strcmp(type, "GT") == 0)
        element_init_GT(Zr, pairing);
    else
        element_init_Zr(Zr, pairing);
    return Zr;
}

element_t newElementRand(pairing_t pairing, char* type) {
    element_t Zr = newElement(pairing, type);
    element_random(Zr);
    return Zr;
}

element_t newElementAssign(pairing_t pairing, char* type, element_t assign_element) {
    element_t Zr = newElement(pairing, type);
    element_set(Zr, assign_element);
    return Zr;
}

element_t newElementNum(pairing_t pairing, char* type, signed long int assign_value) {
    element_t Zr = newElement(pairing, type);
    element_set_si(Zr, assign_value);
    return Zr;
}
