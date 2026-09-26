#ifndef CONVERTER_H
#define CONVERTER_H

#include "automata.h"

namespace automata {

// Convierte un AFN a un AFD equivalente mediante el algoritmo de
// construccion de subconjuntos.

// El AFD resultante tiene:
// - un estado por cada subconjunto relevante de estados del AFN,
// - transiciones deterministas construidas con move + epsilonClosure,
// - estados finales = subconjuntos que contienen al menos un final del AFN.

// Si el AFN no tiene estado inicial definido, se genera una excepcion.
// El AFD puede ser parcial (faltan transiciones para algunos simbolos).

AFD convertirAFNaAFD(const AFN& afn);

}

#endif