#include "pruner.h"

#include <map>
#include <queue>
#include <stdexcept>
#include <sstream>

namespace automata {

// ============================================================
// estadosAlcanzables: BFS desde el estado inicial
// ============================================================
ConjuntoEstados estadosAlcanzables(const AFD& afd) {
    ConjuntoEstados alcanzables;

    // Si no hay estado inicial valido, no hay nada alcanzable
    if (afd.inicial < 0 ||
        static_cast<std::size_t>(afd.inicial) >= afd.estados.size()) {
        return alcanzables;
    }

    std::queue<int> frontera;
    alcanzables.insert(afd.inicial);
    frontera.push(afd.inicial);

    while (!frontera.empty()) {
        int q = frontera.front();
        frontera.pop();

        for (char c : afd.alfabeto) {
            int p = afd.destino(q, c);
            if (p == -1) continue;  // transicion ausente

            // insert devuelve {iterador, bool}; bool es true si se inserto
            if (alcanzables.insert(p).second) {
                frontera.push(p);
            }
        }
    }
    return alcanzables;
}

// ============================================================
// estadosCoAlcanzables: BFS reverso desde los estados finales
// ============================================================
ConjuntoEstados estadosCoAlcanzables(const AFD& afd) {
    ConjuntoEstados coAlcanzables;

    // Construir grafo reverso: destino -> lista de predecesores
    std::map<int, std::vector<int>> aristasReversas;
    for (const auto& fila : afd.transiciones) {
        int q = fila.first;
        for (const auto& celda : fila.second) {
            int p = celda.second;
            aristasReversas[p].push_back(q);
        }
    }

    // Inicializar la frontera con los estados finales
    std::queue<int> frontera;
    for (int f : afd.finales) {
        if (coAlcanzables.insert(f).second) {
            frontera.push(f);
        }
    }

    // BFS reverso
    while (!frontera.empty()) {
        int p = frontera.front();
        frontera.pop();

        auto it = aristasReversas.find(p);
        if (it == aristasReversas.end()) continue;

        for (int q : it->second) {
            if (coAlcanzables.insert(q).second) {
                frontera.push(q);
            }
        }
    }
    return coAlcanzables;
}

// ============================================================
// verificarIntegridad: validaciones estructurales
// ============================================================
void verificarIntegridad(const AFD& afd) {
    if (afd.estados.empty()) {
        throw std::invalid_argument("AFD vacio: no tiene estados.");
    }
    if (afd.inicial < 0 ||
        static_cast<std::size_t>(afd.inicial) >= afd.estados.size()) {
        throw std::invalid_argument(
            "Estado inicial no pertenece al conjunto de estados.");
    }
    for (int f : afd.finales) {
        if (f < 0 || static_cast<std::size_t>(f) >= afd.estados.size()) {
            throw std::invalid_argument(
                "Estado final no pertenece al conjunto de estados.");
        }
    }
    for (const auto& fila : afd.transiciones) {
        int q = fila.first;
        if (q < 0 || static_cast<std::size_t>(q) >= afd.estados.size()) {
            std::ostringstream oss;
            oss << "Transicion con origen invalido: " << q << ".";
            throw std::invalid_argument(oss.str());
        }
        for (const auto& celda : fila.second) {
            char c = celda.first;
            int p = celda.second;

            if (p < 0 || static_cast<std::size_t>(p) >= afd.estados.size()) {
                std::ostringstream oss;
                oss << "Transicion con destino invalido: d(" << q
                    << ", '" << c << "') -> " << p << ".";
                throw std::invalid_argument(oss.str());
            }
            if (afd.alfabeto.find(c) == afd.alfabeto.end()) {
                std::ostringstream oss;
                oss << "Simbolo '" << c << "' no pertenece al alfabeto.";
                throw std::invalid_argument(oss.str());
            }
        }
    }
}

// ============================================================
// podarAFD: elimina inalcanzables y muertos
// ============================================================
AFD podarAFD(const AFD& afd) {
    verificarIntegridad(afd);

    AFD out;
    out.alfabeto = afd.alfabeto;

    ConjuntoEstados alcanzables   = estadosAlcanzables(afd);
    ConjuntoEstados coAlcanzables = estadosCoAlcanzables(afd);

    // ------------------------------------------------------------
    // Caso degenerado 1: ningun estado alcanza un final
    // (lenguaje vacio)
    // ------------------------------------------------------------
    if (coAlcanzables.empty()) {
        out.estados = {afd.estados[afd.inicial]};
        out.inicial = 0;
        out.finales = {};
        return out;
    }

    // ------------------------------------------------------------
    // Conservar estados alcanzables Y co-alcanzables
    // ------------------------------------------------------------
    ConjuntoEstados conservar;
    for (int s : alcanzables) {
        if (coAlcanzables.find(s) != coAlcanzables.end()) {
            conservar.insert(s);
        }
    }

    // ------------------------------------------------------------
    // Caso degenerado 2: el inicial no es co-alcanzable
    // (lenguaje vacio, pero el inicial si es alcanzable)
    // ------------------------------------------------------------
    if (conservar.empty()) {
        out.estados = {afd.estados[afd.inicial]};
        out.inicial = 0;
        out.finales = {};
        return out;
    }

    // ------------------------------------------------------------
    // Mapeo viejo ID -> nuevo ID (renumerar 0..n-1)
    // ------------------------------------------------------------
    std::map<int, int> mapaIds;
    int siguienteId = 0;
    for (int viejo : conservar) {
        mapaIds[viejo] = siguienteId++;
    }

    // Construir el nuevo vector de estados (subconjuntos AFN)
    out.estados.resize(mapaIds.size());
    for (const auto& par : mapaIds) {
        out.estados[par.second] = afd.estados[par.first];
    }

    // Estado inicial
    out.inicial = mapaIds[afd.inicial];

    // Estados finales (remapeados)
    for (int f : afd.finales) {
        auto it = mapaIds.find(f);
        if (it != mapaIds.end()) {
            out.finales.insert(it->second);
        }
    }

    // Transiciones (remapeadas, solo entre estados conservados)
    for (const auto& fila : afd.transiciones) {
        int qViejo = fila.first;
        auto itQ = mapaIds.find(qViejo);
        if (itQ == mapaIds.end()) continue;

        for (const auto& celda : fila.second) {
            char c = celda.first;
            int pViejo = celda.second;
            auto itP = mapaIds.find(pViejo);
            if (itP == mapaIds.end()) continue;

            out.agregarTransicion(itQ->second, c, itP->second);
        }
    }

    return out;
}

// ============================================================
// aceptaCadena: simula la ejecucion de una cadena
// ============================================================
bool aceptaCadena(const AFD& afd, const std::string& entrada) {
    if (afd.inicial < 0) return false;

    int actual = afd.inicial;
    for (char c : entrada) {
        int siguiente = afd.destino(actual, c);
        if (siguiente == -1) {
            return false;  // transicion indefinida -> rechazo
        }
        actual = siguiente;
    }
    return afd.finales.count(actual) > 0;
}

// ============================================================
// lenguajeEquivalente: comparacion sobre muestras
// ============================================================
bool lenguajeEquivalente(const AFD& a, const AFD& b,
                         const std::vector<std::string>& muestras) {
    for (const std::string& w : muestras) {
        if (aceptaCadena(a, w) != aceptaCadena(b, w)) {
            return false;
        }
    }
    return true;
}

}