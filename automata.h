#ifndef AUTOMATA_H
#define AUTOMATA_H

#include <iosfwd>
#include <map>
#include <set>
#include <vector>

namespace automata {

using ConjuntoEstados = std::set<int>;

// Datos publicos para construir y consultar el automata directamente
// Los estados son no negativos, -1 significa inicial sin definir
struct AFN {
    ConjuntoEstados estados;
    std::set<char> alfabeto;
    int inicial = -1;
    ConjuntoEstados finales;
    std::map<int, std::map<char, ConjuntoEstados>> transiciones;
    std::map<int, ConjuntoEstados> epsilon;

    // Tambien rgistran los estados y simbolos utilizados
    void agregarTransicion(int origen, char simbolo, int destino);
    void agregarEpsilon(int origen, int destino);
    ConjuntoEstados destinos(int origen, char simbolo) const;
    ConjuntoEstados destinosEpsilon(int origen) const;
    void imprimirTabla(std::ostream& salida) const;
};

struct AFD {
    // El indice es el ID el contenido es el subconjunto del AFN
    // Por ejemplo: estados[0] = {0,1}, estados[1] = {2}
    std::vector<ConjuntoEstados> estados;
    std::set<char> alfabeto;
    int inicial = -1;
    ConjuntoEstados finales;
    std::map<int, std::map<char, int>> transiciones;

    int buscarEstado(const ConjuntoEstados& conjunto) const; // -1 si no existe
    int agregarEstado(const ConjuntoEstados& conjunto); // Reutiliza duplicados
    void agregarTransicion(int origen, char simbolo, int destino);
    int destino(int origen, char simbolo) const; // -1 si no hay transicion
    void imprimirTabla(std::ostream& salida) const;
};

}

#endif
