#include "../thompson.h"
#include "../operations.h"

#include <cassert>
#include <iostream>

using namespace automata;

// caso base, un simbolo produce exactamente 2 estados
void test_simbolo() {
    AFN afn = convertirRegexAAFN("a");

    assert(afn.inicial != -1);
    assert(afn.estados.size() == 2);
    assert(afn.alfabeto.count('a') == 1);
    assert(afn.finales.size() == 1);

    std::cout << "test_simbolo: OK\n";
}

// concatenacion de 2 simbolos, une 2 fragmentos de 2 estados via epsilon
void test_concatenacion() {
    AFN afn = convertirRegexAAFN("ab");

    assert(afn.estados.size() == 4);
    assert(afn.alfabeto.count('a') == 1);
    assert(afn.alfabeto.count('b') == 1);

    std::cout << "test_concatenacion: OK\n";
}

// union crea un solo inicio y un solo final compartidos por ambas ramas
void test_union() {
    AFN afn = convertirRegexAAFN("a|b");

    assert(afn.alfabeto.count('a') == 1);
    assert(afn.alfabeto.count('b') == 1);
    assert(afn.finales.size() == 1);

    std::cout << "test_union: OK\n";
}

// cierre de kleene, debe permitir 0 o mas repeticiones
void test_estrella() {
    AFN afn = convertirRegexAAFN("a*");

    assert(afn.alfabeto.count('a') == 1);
    assert(afn.finales.size() == 1);

    std::cout << "test_estrella: OK\n";
}

// parentesis agrupan una subexpresion antes de aplicarle el operador
void test_parentesis() {
    AFN afn = convertirRegexAAFN("(a|b)*");

    assert(afn.alfabeto.count('a') == 1);
    assert(afn.alfabeto.count('b') == 1);
    assert(afn.finales.size() == 1);

    std::cout << "test_parentesis: OK\n";
}

// regex vacia debe lanzar excepcion, no construir un afn a medias
void test_regex_invalida() {
    bool lanzoExcepcion = false;

    try {
        convertirRegexAAFN("");
    } catch (const std::invalid_argument&) {
        lanzoExcepcion = true;
    }

    assert(lanzoExcepcion);

    std::cout << "test_regex_invalida: OK\n";
}

int main() {
    test_simbolo();
    test_concatenacion();
    test_union();
    test_estrella();
    test_parentesis();
    test_regex_invalida();

    std::cout << "\nTodas las pruebas de Thompson pasaron.\n";

    return 0;
}