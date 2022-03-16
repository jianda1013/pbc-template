#ifndef PBC_H
#define PBC_H

#include <pbc.h>

void newElement(element_t e, pairing_t pairing, char* type) {
    if (strcmp(type, "G1") == 0)
        element_init_G1(e, pairing);
    else if (strcmp(type, "G2") == 0)
        element_init_G2(e, pairing);
    else if (strcmp(type, "GT") == 0)
        element_init_GT(e, pairing);
    else
        element_init_Zr(e, pairing);
    return;
}

void newElementRand(element_t e, pairing_t pairing, char* type) {
    newElement(e, pairing, type);
    element_random(e);
    return;
}

void newElementAssign(element_t e, pairing_t pairing, char* type, element_t assign_element) {
    newElement(e, pairing, type);
    element_set(e, assign_element);
    return;
}

void newElementNum(element_t e, pairing_t pairing, char* type, signed long int assign_value) {
    newElement(e, pairing, type);
    element_set_si(e, assign_value);
    return;
}
