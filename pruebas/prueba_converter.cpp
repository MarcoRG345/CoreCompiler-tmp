#include "../converter.h"
#include "../thompson.h"

#include <cassert>
#include <iostream>

using namespace automata;

// "a" es el caso mas simple, un afn de 2 estados sin epsilons
void test_conversion_simple() {
    AFN afn = convertirRegexAAFN("a");
    AFD afd = convertirAFNaAFD(afn);

    assert(afd.estados.size() == 2);
    assert(afd.inicial == 0);
    assert(afd.finales.size() == 1);

    std::cout << "test_conversion_simple: OK\n";
}

// "a|b" fuerza la union, revisa que ambos simbolos queden en el alfabeto
void test_conversion_union() {
    AFN afn = convertirRegexAAFN("a|b");
    AFD afd = convertirAFNaAFD(afn);

    assert(afd.estados.size() >= 2);
    assert(afd.alfabeto.count('a') == 1);
    assert(afd.alfabeto.count('b') == 1);
    assert(!afd.finales.empty());

    std::cout << "test_conversion_union: OK\n";
}

// "a*" fuerza el cierre de kleene, el estado inicial tambien debe ser final
void test_conversion_estrellla() {
    AFN afn = convertirRegexAAFN("a*");
    AFD afd = convertirAFNaAFD(afn);

    assert(!afd.estados.empty());
    assert(afd.alfabeto.count('a') == 1);
    assert(!afd.finales.empty());

    std::cout << "test_conversion_estrellla: OK\n";
}

int main() {
    test_conversion_simple();
    test_conversion_union();
    test_conversion_estrellla();

    std::cout << "\nTodas las pruebas del conversor pasaron.\n";

    return 0;
}