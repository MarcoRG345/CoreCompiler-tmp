#include "automata.h"
#include "converter.h"
#include "minimizer.h"
#include "pruner.h"
#include "thompson.h"

#include <cassert>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

using namespace automata;

namespace {

struct CasoDePrueba {
    std::string regex;
    std::vector<std::string> aceptadas;
    std::vector<std::string> rechazadas;
};

std::pair<int, int> correrCaso(const CasoDePrueba& caso) {
    std::cout << "\n================================================\n";
    std::cout << "Expresion regular: " << caso.regex << "\n";
    std::cout << "================================================\n";

    // Regex -> AFN -> AFD
    AFN afn = convertirRegexAAFN(caso.regex);
    AFD crudo = convertirAFNaAFD(afn);

    // Eliminar estados que no son necesarios antes de Hopcroft.
    AFD original = podarAFD(crudo);

    std::cout << "\n--- AFD original (tras poda) ---\n";
    original.imprimirTabla(std::cout);

    // Minimización mediante refinamiento de particiones de Hopcroft.
    AFD minimo = minimizarAFD(original);

    std::cout << "\n--- AFD minimizado ---\n";
    minimo.imprimirTabla(std::cout);

    // Comprobación requerida por la práctica.
    std::cout << "\nEstados originales: "
              << original.estados.size()
              << " | Estados minimizados: "
              << minimo.estados.size() << "\n";

    assert(original.estados.size() >= minimo.estados.size());

    std::cout << "Comprobacion |Q| >= |Q'|: OK\n";

    int aciertos = 0;
    int total = static_cast<int>(
        caso.aceptadas.size() + caso.rechazadas.size()
    );

    std::cout << "\n[Cadenas que deben ser ACEPTADAS]\n";

    for (const auto& s : caso.aceptadas) {
        bool resultado = aceptaCadena(minimo, s);

        std::cout << "  \"" << s << "\" -> "
                  << (resultado ? "ACEPTADA" : "RECHAZADA")
                  << (resultado ? "  [PASS]" : "  [FAIL]")
                  << "\n";

        if (resultado) {
            ++aciertos;
        }
    }

    std::cout << "\n[Cadenas que deben ser RECHAZADAS]\n";

    for (const auto& s : caso.rechazadas) {
        bool resultado = aceptaCadena(minimo, s);

        std::cout << "  \"" << s << "\" -> "
                  << (resultado ? "ACEPTADA" : "RECHAZADA")
                  << (!resultado ? "  [PASS]" : "  [FAIL]")
                  << "\n";

        if (!resultado) {
            ++aciertos;
        }
    }

    std::cout << "\nResultado del caso: "
              << aciertos << "/" << total
              << " pruebas superadas.\n";

    return {aciertos, total};
}

} // namespace

int main() {

    const std::vector<CasoDePrueba> casos = {
        {
            "(a|b)*abb",
            {
                "abb",
                "aabb",
                "babb",
                "ababb",
                "bbabb",
                "aaabb",
                "abababb",
                "bababb",
                "aabbabb",
                "aababb"
            },
            {
                "",
                "a",
                "ab",
                "abbb",
                "abba",
                "aabbb",
                "ba",
                "aabab",
                "abab",
                "aab"
            }
        },

        {
            "(0|1)*01(0|1)*",
            {
                "01",
                "010",
                "101",
                "0011",
                "1101",
                "0101",
                "1010",
                "0001",
                "1001",
                "110011"
            },
            {
                "",
                "0",
                "1",
                "00",
                "11",
                "10",
                "100",
                "110",
                "1110000",
                "111100000"
            }
        },

        {
            "(0|10*1)*",
            {
                "",
                "0",
                "11",
                "00",
                "110",
                "101",
                "1010",
                "1111",
                "010010",
                "1100110"
            },
            {
                "1",
                "01",
                "10",
                "111",
                "100",
                "010",
                "0001",
                "1101",
                "11100",
                "0110100"
            }
        }
    };

    int aciertosTotales = 0;
    int totalGeneral = 0;

    for (const auto& caso : casos) {
        auto resultado = correrCaso(caso);

        aciertosTotales += resultado.first;
        totalGeneral += resultado.second;
    }

    std::cout << "\n================================================\n";
    std::cout << "RESULTADO GLOBAL: "
              << aciertosTotales << "/" << totalGeneral
              << " pruebas superadas.\n";
    std::cout << "================================================\n";

    return (aciertosTotales == totalGeneral) ? 0 : 1;
}
