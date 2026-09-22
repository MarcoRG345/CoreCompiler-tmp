#ifndef PRUNER_H
#define PRUNER_H

#include "automata.h"

#include <string>
#include <vector>

namespace automata {

// ============================================================
// Integracion y poda
// ------------------------------------------------------------
// Responsabilidad:
//   - Recibir el AFD crudo(regex -> AFN -> AFD)
//   - Verificar integridad estructural
//   - Eliminar estados inalcanzables desde el inicial
//   - Eliminar estados muertos (no co-alcanzables hacia finales)
//   - Entregar un AFD listo para Hopcroft
//
// Precondicion de salida (garantizada):
//   - inicial valido
//   - finales subset of estados
//   - Todo estado es alcanzable desde inicial
//   - Todo estado alcanza algun estado final
//     (salvo caso degenerado de lenguaje vacio)
// ============================================================

// Devuelve el conjunto de IDs alcanzables desde el estado inicial (BFS)
ConjuntoEstados estadosAlcanzables(const AFD& afd);

// Devuelve el conjunto de IDs que pueden alcanzar algun estado final
// (BFS reverso sobre el grafo de transiciones)
ConjuntoEstados estadosCoAlcanzables(const AFD& afd);

// Verifica integridad estructural del AFD.
// Lanza std::invalid_argument con mensaje descriptivo si falla.
void verificarIntegridad(const AFD& afd);

// Poda estados inalcanzables y muertos.
// Retorna un AFD con IDs renumerados 0..n-1, listo para Hopcroft.
AFD podarAFD(const AFD& afd);

// Simula la ejecucion de una cadena sobre el AFD.
// Retorna true si la cadena es aceptada; false si es rechazada
// o si encuentra una transicion indefinida.
bool aceptaCadena(const AFD& afd, const std::string& entrada);

// Verifica que dos AFDs reconocen el mismo lenguaje sobre un
// conjunto de cadenas de prueba.
bool lenguajeEquivalente(const AFD& a, const AFD& b,
                         const std::vector<std::string>& muestras);

}  // namespace automata

#endif  // PRUNER_H