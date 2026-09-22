#include "converter.h"
#include "operations.h"

#include <queue>

namespace automata {

namespace {

// Un subconjunto del AFN es final en el AFD si contiene al menos
// un estado final del AFN.
bool esFinal(const ConjuntoEstados& subconjunto, const ConjuntoEstados& finalesAFN) {
    for (int estado : subconjunto) {
        if (finalesAFN.count(estado)) return true;
    }
    return false;
}

}

AFD convertirAFNaAFD(const AFN& afn) {
    AFD afd;
    afd.alfabeto = afn.alfabeto;

    // Sin estado inicial no hay conversion posible: AFD vacio.
    if (afn.inicial == -1) {
        return afd;
    }

    // Subconjunto inicial: clausura epsilon del inicial del AFN.
    const ConjuntoEstados subconjuntoInicial = epsilonClosure(afn, afn.inicial);
    afd.inicial = afd.agregarEstado(subconjuntoInicial);
    if (esFinal(subconjuntoInicial, afn.finales)) {
        afd.finales.insert(afd.inicial);
    }

    // BFS sobre los subconjuntos descubiertos.
    std::queue<int> pendientes;
    pendientes.push(afd.inicial);

    while (!pendientes.empty()) {
        const int idOrigen = pendientes.front();
        pendientes.pop();
        const ConjuntoEstados subconjuntoOrigen = afd.estados[idOrigen];

        for (char simbolo : afn.alfabeto) {
            const ConjuntoEstados movidos = move(afn, subconjuntoOrigen, simbolo);

            // Sin destinos: celda parcial del AFD (destino == -1).
            if (movidos.empty()) {
                continue;
            }

            const ConjuntoEstados subconjuntoDestino = epsilonClosure(afn, movidos);

            // Detectamos si el subconjunto ya existia ANTES de agregarlo.
            const bool yaExistia = (afd.buscarEstado(subconjuntoDestino) != -1);
            const int idDestino = afd.agregarEstado(subconjuntoDestino);

            afd.agregarTransicion(idOrigen, simbolo, idDestino);

            if (esFinal(subconjuntoDestino, afn.finales)) {
                afd.finales.insert(idDestino);
            }

            // Solo encolamos subconjuntos nuevos para explorarlos.
            if (!yaExistia) {
                pendientes.push(idDestino);
            }
        }
    }

    return afd;
}

}  // namespace automata