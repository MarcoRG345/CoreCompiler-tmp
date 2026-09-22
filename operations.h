#ifndef OPERATIONS_H
#define OPERATIONS_H

#include "automata.h"

namespace automata {

// move(T, a): estados alcanzables desde algun estado de T consumiendo el
// simbolo a. Es la union de destinos(estado, a) para cada estado en T.
// Si ningun estado de T tiene transicion con ese simbolo, el resultado
// es el conjunto vacio.
ConjuntoEstados move(const AFN& afn, const ConjuntoEstados& T, char simbolo);

// epsilonClosure(T): todos los estados alcanzables desde T mediante cero
// o mas transiciones epsilon. Siempre incluye los estados originales de T
// (cero transiciones tambien cuenta como alcanzable).
ConjuntoEstados epsilonClosure(const AFN& afn, const ConjuntoEstados& T);

// Sobrecarga de conveniencia para calcular la clausura epsilon de un solo
// estado, por ejemplo epsilonClosure(afn, afn.inicial) al iniciar la
// construccion de subconjuntos.
ConjuntoEstados epsilonClosure(const AFN& afn, int estado);

}

#endif
