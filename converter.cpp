#include "converter.h"
#include "operations.h"

#include <queue>
#include <stdexcept>

namespace automata {

AFD convertirAFNaAFD(const AFN& afn) {
    AFD afd;

    afd.alfabeto = afn.alfabeto;

    // sin inicial no hay conversion posible, se rechaza en vez de devolver vacio
    if (afn.inicial == -1) {
        throw std::invalid_argument("El AFN no tiene estado inicial");
    }

    // subconjunto inicial, clausura epsilon del inicial del afn
    ConjuntoEstados inicial;
    inicial.insert(afn.inicial);
    inicial = epsilonClosure(afn, inicial);

    afd.agregarEstado(inicial);
    afd.inicial = 0;

    // bfs sobre los subconjuntos descubiertos
    std::queue<int> pendientes;
    pendientes.push(0);

    while (!pendientes.empty()) {
        int idActual = pendientes.front();
        pendientes.pop();

        const ConjuntoEstados conjuntoActual = afd.estados[idActual];

        for (char simbolo : afd.alfabeto) {
            ConjuntoEstados movidos = move(afn, conjuntoActual, simbolo);

            // sin destinos, transicion parcial, no se agrega nada
            if (movidos.empty()) {
                continue;
            }

            ConjuntoEstados siguiente = epsilonClosure(afn, movidos);

            int idSiguiente = afd.buscarEstado(siguiente);

            // solo se encola si el subconjunto es nuevo
            if (idSiguiente == -1) {
                idSiguiente = afd.agregarEstado(siguiente);
                pendientes.push(idSiguiente);
            }

            afd.agregarTransicion(idActual, simbolo, idSiguiente);
        }
    }

    // un estado del afd es final si su subconjunto contiene algun final del afn
    for (int i = 0; i < static_cast<int>(afd.estados.size()); ++i) {
        for (int estadoAFN : afd.estados[i]) {
            if (afn.finales.count(estadoAFN)) {
                afd.finales.insert(i);
                break;
            }
        }
    }

    return afd;
}

}