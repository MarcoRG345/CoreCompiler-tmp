#ifndef THOMPSON_H
#define THOMPSON_H

#include "automata.h"

#include <string>

namespace automata {

// Convierte una expresion regular a un AFN equivalente mediante la
// construccion de Thompson.
//
// Gramatica soportada (recursiva-descendente):
//   union   := concat ('|' concat)*
//   concat  := kleene+
//   kleene  := atomo '*'*
//   atomo   := caracter | '(' union ')'
//
// Caracteres validos: cualquier alfanumerico (a-z, A-Z, 0-9). No se
// soportan operadores adicionales (+, ?, clases de caracteres, escapes,
// ni la cadena vacia como literal).
//
// Lanza std::invalid_argument si la expresion es vacia, tiene parentesis
// desbalanceados, caracteres inesperados, o sobran caracteres al final.
AFN convertirRegexAAFN(const std::string& regex);

}  // namespace automata

#endif  // THOMPSON_H
