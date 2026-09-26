#include "automata.h"

#include <limits>
#include <ostream>
#include <stdexcept>

namespace automata {
namespace {

// Escriba {0,1,2}; un conjunto vacio se muestra como {}.
void imprimirConjunto(std::ostream& salida, const ConjuntoEstados& conjunto) {
    salida << '{';
    // Esta bandera evita imprimir una coma antes del primer elemento
    bool primero = true;
    for (int estado : conjunto) {
        if (!primero) salida << ',';
        salida << estado;
        primero = false;
    }
    salida << '}';
}

// Agrega las marcas del estado, si es inicial y final, se imprime, por ejemplo, ->*0.
void imprimirEstado(std::ostream& salida, int estado, int inicial,
                    const ConjuntoEstados& finales) {
    if (estado == inicial) salida << "->";
    // En un set, count devuelve 1 si el elemento existe y 0 si no existe.
    if (finales.count(estado)) salida << '*';
    salida << estado;
}

}

// Registra una transicion que consume un simbolo. Admite varios destinos.
void AFN::agregarTransicion(int origen, char simbolo, int destino) {
    if (origen < 0 || destino < 0) throw std::invalid_argument("Estado negativo");
    // Los conjuntos registran los estados y simbolos sin repetir elementos.
    estados.insert(origen);
    estados.insert(destino);
    alfabeto.insert(simbolo);
    // [] crea las entradas faltantes de los mapas; insert agrega el destino al set.
    transiciones[origen][simbolo].insert(destino);
}

// Epsilon permite cambiar de estado sin consumir entrada y se guarda por separado.
void AFN::agregarEpsilon(int origen, int destino) {
    if (origen < 0 || destino < 0) throw std::invalid_argument("Estado negativo");
    estados.insert(origen);
    estados.insert(destino);
    epsilon[origen].insert(destino);
}

// Consulta un solo paso desde origen con simbolo.
ConjuntoEstados AFN::destinos(int origen, char simbolo) const {
    const auto fila = transiciones.find(origen);
    if (fila == transiciones.end()) return {};
    const auto celda = fila->second.find(simbolo);
    if (celda == fila->second.end()) return {};
    return celda->second;
}

// Obtiene solo los vecinos epsilon directos; no recorre toda la clausura epsilon.
// Si no hay entrada para el origen, devuelve un conjunto vacio
ConjuntoEstados AFN::destinosEpsilon(int origen) const {
    const auto fila = epsilon.find(origen);
    if (fila == epsilon.end()) return {};
    return fila->second;
}

// Presenta una fila por estado y una columna por simbolo, mas la columna epsilon.
void AFN::imprimirTabla(std::ostream& salida) const {
    salida << "AFN (-> inicial, * final, {} sin destinos)\n";
    if (inicial == -1) salida << "Estado inicial: sin definir\n";
    // Encabezado de la tabla. -1 se reserva para un inicial aun sin definir
    salida << "Estado";
    for (char simbolo : alfabeto) salida << " | " << simbolo;
    salida << " | epsilon\n";
    // Cada celda muestra un conjunto porque el AFN puede tener varios destinos
    for (int estado : estados) {
        imprimirEstado(salida, estado, inicial, finales);
        for (char simbolo : alfabeto) {
            salida << " | ";
            imprimirConjunto(salida, destinos(estado, simbolo));
        }
        salida << " | ";
        imprimirConjunto(salida, destinosEpsilon(estado));
        salida << '\n';
    }
}

// Compara conjuntos completos: {0,1} y {1,0} rpresentan el mismo estado.
int AFD::buscarEstado(const ConjuntoEstados& conjunto) const {
    for (std::size_t i = 0; i < estados.size(); ++i) {
        if (estados[i] == conjunto) return static_cast<int>(i);
    }
    return -1; // Ningun subconjunto coincide; 0 si seria un ID valido
}

// Registra un subconjunto nuevo o reutiliza el ID de uno que ya existe
int AFD::agregarEstado(const ConjuntoEstados& conjunto) {
    int id = buscarEstado(conjunto);
    if (id != -1) return id;
    if (estados.size() >= static_cast<std::size_t>(std::numeric_limits<int>::max())) {
        throw std::overflow_error("Demasiados estados");
    }
    id = static_cast<int>(estados.size());
    estados.push_back(conjunto);
    return id;
}

// Los estados del AFD deben registrarse antes mediante agregarEstado()
void AFD::agregarTransicion(int origen, char simbolo, int destino) {
    // Rechaza IDs negativos o fuera del vector
    if (origen < 0 || destino < 0 || static_cast<std::size_t>(origen) >= estados.size()
        || static_cast<std::size_t>(destino) >= estados.size()) {
        throw std::invalid_argument("Estado AFD inexistente");
    }
    // Desde un mismo origen, un simbolo solo puede tener un destino en un AFD
    int anterior = this->destino(origen, simbolo);
    if (anterior != -1 && anterior != destino) {
        throw std::invalid_argument("La transicion ya tiene otro destino");
    }
    alfabeto.insert(simbolo);
    transiciones[origen][simbolo] = destino;
}

// Consulta un destino sin modificar la tabla. -1 significa transicion ausente
int AFD::destino(int origen, char simbolo) const {
    const auto fila = transiciones.find(origen);
    if (fila == transiciones.end()) return -1;
    const auto celda = fila->second.find(simbolo);
    if (celda == fila->second.end()) return -1;
    return celda->second;
}

// Muestra IDs, subconjuntos del AFN y transiciones. El AFD no tiene columna epsilon
void AFD::imprimirTabla(std::ostream& salida) const {
    salida << "AFD (-> inicial, * final, - sin transicion)\n";
    if (inicial == -1) salida << "Estado inicial: sin definir\n";
    salida << "Estado | Subconjunto AFN";
    for (char simbolo : alfabeto) salida << " | " << simbolo;
    salida << '\n';
    // Recorremos el vector para mostrar el ID y el conjunto asociado a ese ID
    for (std::size_t i = 0; i < estados.size(); ++i) {
        int id = static_cast<int>(i);
        imprimirEstado(salida, id, inicial, finales);
        salida << " | ";
        imprimirConjunto(salida, estados[i]);
        for (char simbolo : alfabeto) {
            salida << " | ";
            int siguiente = destino(id, simbolo);
            if (siguiente == -1) salida << '-';
            else salida << siguiente;
        }
        salida << '\n';
    }
}

}
