#ifndef MINIMIZER_H
#define MINIMIZER_H

#include "automata.h"

namespace automata {

// Minimiza un AFD mediante el algoritmo de refinamiento de particiones
// de Hopcroft.
//
// Precondicion: el AFD no debe tener estados inalcanzables (usar
// podarAFD() antes de llamar a esta funcion). El AFD puede ser parcial;
// las transiciones indefinidas se interpretan como rechazo implicito,
// igual que en aceptaCadena().
//
// Postcondicion: el AFD resultante reconoce el mismo lenguaje que el
// original, con |Q'| <= |Q|, y sus IDs de estado quedan renumerados
// 0..n-1 segun el orden de los bloques de la particion final.
AFD minimizarAFD(const AFD& afd);

}

#endif
