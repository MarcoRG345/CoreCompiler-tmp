#include "../pruner.h"

#include <iostream>

using namespace automata;

int main() {
    // ============================================================
    // AFD del ejemplo de la practica:
    //   A=0, B=1, C=2, D=3, E=4
    // Agregamos:
    //   Z=99: estado inalcanzable
    //   M=98: estado muerto
    // ============================================================
    AFD d;
    d.alfabeto = {'0', '1'};
    d.estados = {
        {0},   // 0: A
        {1},   // 1: B
        {2},   // 2: C
        {3},   // 3: D
        {4},   // 4: E
        {99},  // 5: Z (inalcanzable)
        {98},  // 6: M (muerto)
    };
    d.inicial = 0;    // A
    d.finales = {4};  // E

    // Transiciones del DFA original
    d.agregarTransicion(0, '0', 1);  // A--0--> B
    d.agregarTransicion(0, '1', 2);  // A--1--> C
    d.agregarTransicion(1, '0', 1);  // B--0--> B
    d.agregarTransicion(1, '1', 3);  // B--1--> D
    d.agregarTransicion(2, '0', 1);  // C--0--> B
    d.agregarTransicion(2, '1', 2);  // C--1--> C
    d.agregarTransicion(3, '0', 1);  // D--0--> B
    d.agregarTransicion(3, '1', 4);  // D--1--> E
    d.agregarTransicion(4, '0', 1);  // E--0--> B
    d.agregarTransicion(4, '1', 2);  // E--1--> C

    // Z (99) inalcanzable: no tiene entrantes
    d.agregarTransicion(5, '0', 5);
    d.agregarTransicion(5, '1', 5);

    // M (98) muerto: alcanzable pero nunca llega a E
    d.agregarTransicion(6, '0', 6);
    d.agregarTransicion(6, '1', 6);

    // ============================================================
    // Imprimir ANTES de podar
    // ============================================================
    std::cout << "===== ANTES DE PODAR =====\n";
    d.imprimirTabla(std::cout);
    std::cout << "Total de estados: " << d.estados.size() << "\n";

    // ============================================================
    // Podar
    // ============================================================
    AFD podado = podarAFD(d);

    // ============================================================
    // Imprimir DESPUES de podar
    // ============================================================
    std::cout << "\n===== DESPUES DE PODAR =====\n";
    podado.imprimirTabla(std::cout);
    std::cout << "Total de estados: " << podado.estados.size() << "\n";

    // ============================================================
    // Verificaciones
    // ============================================================
    std::cout << "\n===== VERIFICACIONES =====\n";
    std::cout << "|Q_original| = " << d.estados.size() << "\n";
    std::cout << "|Q_podado|   = " << podado.estados.size() << "\n";
    std::cout << "|Q_original| >= |Q_podado| -> "
              << (d.estados.size() >= podado.estados.size() ? "OK" : "FALLO")
              << "\n";

    std::vector<std::string> muestras = {
        "", "0", "1", "00", "01", "10", "11",
        "011", "101", "0101", "1101", "0111"
    };
    std::cout << "Equivalencia de lenguaje -> "
              << (lenguajeEquivalente(d, podado, muestras) ? "OK" : "FALLO")
              << "\n";

    return 0;
}