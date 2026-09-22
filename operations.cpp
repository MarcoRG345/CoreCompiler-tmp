#include "operations.h"

#include <vector>

namespace automata {

// Recorre cada estado de T y une sus destinos con ese simbolo. destinos()
// ya devuelve un conjunto vacio cuando no hay transicion, asi que no hace
// falta comprobar nada especial antes de insertar.
ConjuntoEstados move(const AFN& afn, const ConjuntoEstados& T, char simbolo) {
    ConjuntoEstados resultado;
    for (int estado : T) {
        const ConjuntoEstados destinos = afn.destinos(estado, simbolo);
        resultado.insert(destinos.begin(), destinos.end());
    }
    return resultado;
}

// Recorre las transiciones epsilon en a partiendo de T,
// usando una pila explicita. Cada estado se procesa
// una sola vez: al insertarlo en "resultado" tambien queda marcado como
// visitado, lo que evita ciclos infinitos cuando el AFN tiene epsilon-ciclos
// (por ejemplo 0 -> 2 -> 0).
ConjuntoEstados epsilonClosure(const AFN& afn, const ConjuntoEstados& T) {
    // El resultado siempre incluye los estados originales, aunque no
    // tengan transiciones epsilon salientes.
    ConjuntoEstados resultado = T;
    std::vector<int> pendientes(T.begin(), T.end());

    while (!pendientes.empty()) {
        int estado = pendientes.back();
        pendientes.pop_back();
        for (int vecino : afn.destinosEpsilon(estado)) {
            // set::insert devuelve {iterador, bool}; el bool es true solo
            // si el elemento no estaba antes en el conjunto. Asi solo se
            // vuelve a explorar un estado la primera vez que se descubre.
            if (resultado.insert(vecino).second) {
                pendientes.push_back(vecino);
            }
        }
    }
    return resultado;
}

// Delega en la version de conjuntos para no duplicar la logica de recorrido.
ConjuntoEstados epsilonClosure(const AFN& afn, int estado) {
    return epsilonClosure(afn, ConjuntoEstados{estado});
}

}
