/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : validator.h
*/

#include <stdbool.h>

#define ALPHABET_CARDINALITY 7

typedef enum {
    START,
    S,
    TL,
    V,
    H,
    STL,
    SV,
    TLV,
    STLV,
    TS,
    __VALID_END_STATES,
    F,
    N,
    FS,
    FTL,
    FV,
    SN,
    TLN,
    NV,
    FSV,
    FSTL,
    FTLV,
    SNV,
    STLN,
    TLNV,
    FSTLV,
    STLNV,
    __LAST_STATE
} FSM_STATES;

FSM_STATES delta(FSM_STATES, char);

bool isFinal(FSM_STATES);
