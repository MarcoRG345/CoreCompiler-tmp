#include "../minimizer.h"
#include "../pruner.h"
#include <cassert>
#include <iostream>

using namespace automata;

// estados 1 y 2 son equivalentes (mismas transiciones a/b -> 3)
// se espera que hopcroft los fusione, 4 estados pasan a 3
AFD construirAFDConEstadosEquivalentes() {
    AFD d;

    d.alfabeto = {'a', 'b'};
    d.estados = {{0}, {1}, {2}, {3}};
    d.inicial = 0;
    d.finales = {3};

    d.agregarTransicion(0, 'a', 1);
    d.agregarTransicion(0, 'b', 2);

    d.agregarTransicion(1, 'a', 3);
    d.agregarTransicion(1, 'b', 3);

    d.agregarTransicion(2, 'a', 3);
    d.agregarTransicion(2, 'b', 3);

    d.agregarTransicion(3, 'a', 3);
    d.agregarTransicion(3, 'b', 3);

    return d;
}

// afd ya minimo (0 y 1 no son equivalentes), sirve de caso de control
AFD construirAFDMinimo() {
    AFD d;

    d.alfabeto = {'a', 'b'};
    d.estados = {{0}, {1}};
    d.inicial = 0;
    d.finales = {1};

    d.agregarTransicion(0, 'a', 1);
    d.agregarTransicion(0, 'b', 0);

    d.agregarTransicion(1, 'a', 1);
    d.agregarTransicion(1, 'b', 0);

    return d;
}

// verifica que si se reduce, y a exactamente 3 estados
void test_estados_equivalentes() {
    AFD d = construirAFDConEstadosEquivalentes();

    AFD minimo = minimizarAFD(d);

    assert(minimo.estados.size() < d.estados.size());
    assert(minimo.estados.size() == 3);

    std::cout << "test_estados_equivalentes: OK\n";
}

// un afd ya minimo no debe perder ni ganar estados
void test_dfa_ya_minimo() {
    AFD d = construirAFDMinimo();

    AFD minimo = minimizarAFD(d);

    assert(minimo.estados.size() == d.estados.size());

    std::cout << "test_dfa_ya_minimo: OK\n";
}

// el inicial remapeado debe seguir siendo un indice valido
void test_estado_inicial() {
    AFD d = construirAFDConEstadosEquivalentes();

    AFD minimo = minimizarAFD(d);

    assert(minimo.inicial >= 0);
    assert(minimo.inicial < static_cast<int>(minimo.estados.size()));

    std::cout << "test_estado_inicial: OK\n";
}

// los finales remapeados tambien deben ser indices validos y no vacios
void test_estados_finales() {
    AFD d = construirAFDConEstadosEquivalentes();

    AFD minimo = minimizarAFD(d);

    assert(!minimo.finales.empty());

    for (int estadoFinal : minimo.finales) {
        assert(estadoFinal >= 0);
        assert(estadoFinal < static_cast<int>(minimo.estados.size()));
    }

    std::cout << "test_estados_finales: OK\n";
}

// el lenguaje debe ser el mismo antes y despues de minimizar
void test_aceptacion_conservada() {
    AFD d = construirAFDConEstadosEquivalentes();

    AFD minimo = minimizarAFD(d);

    assert(aceptaCadena(d, "aa"));
    assert(aceptaCadena(minimo, "aa"));

    assert(aceptaCadena(d, "ab"));
    assert(aceptaCadena(minimo, "ab"));

    assert(aceptaCadena(d, "ba"));
    assert(aceptaCadena(minimo, "ba"));

    assert(!aceptaCadena(d, ""));
    assert(!aceptaCadena(minimo, ""));

    std::cout << "test_aceptacion_conservada: OK\n";
}

int main() {
    test_estados_equivalentes();
    test_dfa_ya_minimo();
    test_estado_inicial();
    test_estados_finales();
    test_aceptacion_conservada();

    std::cout << "\nTodas las pruebas de minimizacion pasaron.\n";

    return 0;
}