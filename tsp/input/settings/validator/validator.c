/*
* Project  : Travelling Salesman Problem implementations
* Authors  : Luigi Frigione, Daniel Carlesso
* IDs      : 2060685, 2088626
* File     : validator.c
*/

#include "validator.h"
#include <stdio.h>

static FSM_STATES MACHINE [__LAST_STATE][ALPHABET_CARDINALITY] = {
    /* 0 1 2 3 4 5 */
    {TS, F, S, TL, N, V, H}, /* START */
    {TS, FS, TS, STL, SN, SV, TS}, /* S */
    {TS, FTL, STL, TS, TLN, TLV, TS}, /* TL */
    {TS, FV, SV, TLV, NV, TS, TS}, /* V */
    {TS, TS, TS, TS, TS, TS, TS}, /* H */
    {TS, FSTL, TS, TS, STLN, STLV, TS}, /* STL */
    {TS, FSV, TS, STLV, SNV, TS, TS}, /* SV */
    {TS, FTLV, STLV, TS, TLNV, TS, TS}, /* TLV */
    {TS, FSTLV, TS, TS, STLNV, TS, TS}, /* STLV */
    {TS, TS, TS, TS, TS, TS, TS}, /* TS */
    {TS, TS, TS, TS, TS, TS, TS}, /* __VALID_END_STATES - Dummy */
    {TS, TS, FS, FTL, TS, FV, TS}, /* F */
    {TS, TS, SN, TLN, TS, NV, TS}, /* N */
    {TS, TS, TS, FSTL, TS, FSV, TS}, /* FS */
    {TS, TS, FSTL, TS, TS, FTLV, TS}, /* FTL */
    {TS, TS, FSV, FTLV, TS, TS, TS}, /* FV */
    {TS, TS, TS, STLN, TS, SNV, TS}, /* SN */
    {TS, TS, STLN, TS, TS, TLNV, TS}, /* TLN */
    {TS, TS, SNV, TLNV, TS, TS, TS}, /* NV */
    {TS, TS, TS, FSTLV, TS, TS, TS}, /* FSV */
    {TS, TS, TS, TS, TS, FSTLV, TS}, /* FSTL */
    {TS, TS, FSTLV, TS, TS, TS, TS}, /* FTLV */
    {TS, TS, TS, STLNV, TS, TS, TS}, /* SNV */
    {TS, TS, TS, TS, TS, STLNV, TS}, /* STLN */
    {TS, TS, STLNV, TS, TS, TS, TS}, /* TLNV */
    {TS, TS, TS, TS, TS, TS, TS}, /* FSTLV */
    {TS, TS, TS, TS, TS, TS, TS}, /* STLNV */
    /* {TS, TS, TS, TS, TS, TS} */ /* __LAST_STATE - Dummy */ /* It is not necessary, we have already instantiated __LAST_STATE rows */
};

/*
* IP curr current state
* IP symbol input symbol
* OR next state
*/
FSM_STATES delta(FSM_STATES curr, char symbol){

    return MACHINE[curr][(int)symbol];

}/* delta */

/*
* IP s state
* OR true if is a final state, false otherwise
*/
bool isFinal(FSM_STATES s){
	return __VALID_END_STATES < s && s < __LAST_STATE;
}/* isFinal */
