#include "../pruner.h"

#include <cassert>
#include <iostream>

using namespace automata;

// ============================================================
// Helper: AFD base sin estados redundantes
// Acepta cadenas que terminan en 'b'
// ============================================================
AFD construirAFDBase() {
    AFD d;
    d.alfabeto = {'a', 'b'};
    d.estados  = {{0}, {1}, {2}};
    d.inicial  = 0;
    d.finales  = {2};

    d.agregarTransicion(0, 'a', 1);
    d.agregarTransicion(0, 'b', 2);
    d.agregarTransicion(1, 'a', 1);
    d.agregarTransicion(1, 'b', 2);
    d.agregarTransicion(2, 'a', 2);
    d.agregarTransicion(2, 'b', 2);
    return d;
}

// ============================================================
// Caso 1: AFD ya limpio (no debe podar nada)
// ============================================================
void test_sin_poda() {
    std::cout << "[TEST] AFD ya limpio...\n";
    AFD d = construirAFDBase();
    AFD podado = podarAFD(d);

    assert(podado.estados.size() == 3);
    assert(podado.inicial == 0);
    assert(podado.finales.size() == 1);
    assert(podado.finales.count(2) == 1);
    std::cout << "  OK\n";
}

// ============================================================
// Caso 2: Estado inalcanzable (debe eliminarse)
// ============================================================
void test_inalcanzable_eliminado() {
    std::cout << "[TEST] Estado inalcanzable...\n";
    AFD d = construirAFDBase();

    // Agregar estado 3 al que nadie apunta
    d.estados.push_back({3});
    d.agregarTransicion(3, 'a', 3);
    d.agregarTransicion(3, 'b', 3);

    AFD podado = podarAFD(d);

    // Solo deben quedar 3 estados (0, 1, 2)
    assert(podado.estados.size() == 3);
    std::cout << "  OK\n";
}

// ============================================================
// Caso 3: Estado muerto (debe eliminarse)
// ============================================================
void test_muerto_eliminado() {
    std::cout << "[TEST] Estado muerto...\n";
    AFD d = construirAFDBase();

    // Agregar estado 3 alcanzable pero sin camino a final
    d.estados.push_back({3});
    // Redirigir 1--b--> 3 en lugar de 1--b--> 2
    d.transiciones[1]['b'] = 3;
    d.agregarTransicion(3, 'a', 3);
    d.agregarTransicion(3, 'b', 3);

    AFD podado = podarAFD(d);

    // El estado 3 es muerto -> se elimina
    // Quedan 0, 1, 2 -> 3 estados
    assert(podado.estados.size() == 2);
    std::cout << "  OK\n";
}

// ============================================================
// Caso 4: Lenguaje vacio (1 estado no aceptante)
// ============================================================
void test_lenguaje_vacio() {
    std::cout << "[TEST] Lenguaje vacio...\n";
    AFD d;
    d.alfabeto = {'a'};
    d.estados  = {{0}, {1}};
    d.inicial  = 0;
    d.finales  = {};  // sin finales

    d.agregarTransicion(0, 'a', 1);
    d.agregarTransicion(1, 'a', 1);

    AFD podado = podarAFD(d);

    assert(podado.estados.size() == 1);
    assert(podado.inicial == 0);
    assert(podado.finales.empty());
    std::cout << "  OK\n";
}

// ============================================================
// Caso 5: AFD malformado (debe lanzar excepcion)
// ============================================================
void test_integridad_rechaza_malformado() {
    std::cout << "[TEST] AFD malformado...\n";
    AFD d;
    d.estados  = {{0}, {1}};
    d.alfabeto = {'a'};
    d.inicial  = 0;
    d.finales  = {1};

    // Insertar transicion con destino fuera de rango directamente
    d.transiciones[0]['a'] = 99;

    bool capturado = false;
    try {
        verificarIntegridad(d);
    } catch (const std::invalid_argument& e) {
        capturado = true;
        std::cout << "  Excepcion: " << e.what() << "\n";
    }
    assert(capturado);
    std::cout << "  OK\n";
}

// ============================================================
// Caso 6: Equivalencia de lenguaje antes/despues de podar
// ============================================================
void test_equivalencia_lenguaje() {
    std::cout << "[TEST] Equivalencia de lenguaje...\n";
    AFD d = construirAFDBase();

    // Agregar estado inalcanzable para forzar poda
    d.estados.push_back({3});
    d.agregarTransicion(3, 'a', 3);
    d.agregarTransicion(3, 'b', 3);

    AFD podado = podarAFD(d);

    std::vector<std::string> muestras = {
        "", "a", "aa", "ab", "ba", "bb",
        "aab", "aba", "abb", "bab", "aabb", "abab"
    };
    assert(lenguajeEquivalente(d, podado, muestras));
    std::cout << "  OK\n";
}

// ============================================================
// Caso 7: aceptaCadena sobre AFD base
// ============================================================
void test_acepta_cadena() {
    std::cout << "[TEST] aceptaCadena...\n";
    AFD d = construirAFDBase();

    // El AFD base acepta cadenas que terminan en 'b'
    assert(aceptaCadena(d, "b"));
    assert(aceptaCadena(d, "ab"));
    assert(aceptaCadena(d, "aab"));
    assert(aceptaCadena(d, "bb"));

    assert(!aceptaCadena(d, ""));
    assert(!aceptaCadena(d, "a"));
    assert(!aceptaCadena(d, "aa"));

    std::cout << "  OK\n";
}

// ============================================================
// Main
// ============================================================
int main() {
    std::cout << "===== PRUEBAS UNITARIAS DE P1 (pruner) =====\n\n";

    test_sin_poda();
    test_inalcanzable_eliminado();
    test_muerto_eliminado();
    test_lenguaje_vacio();
    test_integridad_rechaza_malformado();
    test_equivalencia_lenguaje();
    test_acepta_cadena();

    std::cout << "\n[TODAS LAS PRUEBAS DE P1 PASARON]\n";
    return 0;
}