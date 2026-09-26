#include "thompson.h"

#include <cctype>
#include <stdexcept>

namespace automata {
namespace {

// un fragmento siempre tiene un unico inicio y un unico final
// se mantiene igual en cada regla (simbolo, concat, union, estrella)
struct Fragmento {
    int inicio;
    int fin;
};

// agrupa el estado mutable del parser para no pasar varios parametros sueltos
struct EstadoParser {
    const std::string& entrada;
    std::size_t pos;
    int siguienteId;
    AFN& afn;

    // char actual sin consumir, nulo si ya no hay mas entrada
    char actual() const {
        return pos < entrada.size() ? entrada[pos] : '\0';
    }
    // consume el char actual
    void avanzar() { ++pos; }
    // reparte ids consecutivos para los estados del afn
    int nuevoEstado() { return siguienteId++; }
};

// solo se aceptan letras y digitos como simbolos del alfabeto
bool esCaracterValido(char c) {
    return std::isalnum(static_cast<unsigned char>(c)) != 0;
}

// un atomo puede empezar con parentesis o caracter valido
bool esInicioDeAtomo(char c) {
    return c == '(' || esCaracterValido(c);
}

// simbolo suelto, fragmento de 2 estados unidos por una sola transicion
Fragmento simbolo(EstadoParser& ep, char c) {
    int s = ep.nuevoEstado();
    int f = ep.nuevoEstado();
    ep.afn.agregarTransicion(s, c, f);
    return {s, f};
}

// el final de a se conecta por epsilon al inicio de b
Fragmento concatenar(EstadoParser& ep, Fragmento a, Fragmento b) {
    ep.afn.agregarEpsilon(a.fin, b.inicio);
    return {a.inicio, b.fin};
}

// nuevo inicio y final que se ramifican hacia a y b por epsilon
Fragmento unir(EstadoParser& ep, Fragmento a, Fragmento b) {
    int s = ep.nuevoEstado();
    int f = ep.nuevoEstado();
    ep.afn.agregarEpsilon(s, a.inicio);
    ep.afn.agregarEpsilon(s, b.inicio);
    ep.afn.agregarEpsilon(a.fin, f);
    ep.afn.agregarEpsilon(b.fin, f);
    return {s, f};
}

// cierre de kleene, lazo de vuelta al inicio mas atajo directo para la cadena vacia
Fragmento estrella(EstadoParser& ep, Fragmento a) {
    int s = ep.nuevoEstado();
    int f = ep.nuevoEstado();
    ep.afn.agregarEpsilon(s, a.inicio);
    ep.afn.agregarEpsilon(s, f);
    ep.afn.agregarEpsilon(a.fin, f);
    ep.afn.agregarEpsilon(a.fin, a.inicio);
    return {s, f};
}

// declaracion adelantada, atomo y union son mutuamente recursivos
Fragmento parsearUnion(EstadoParser& ep);

// atomo = caracter suelto o subexpresion entre parentesis
Fragmento parsearAtomo(EstadoParser& ep) {
    if (ep.actual() == '(') {
        ep.avanzar();
        Fragmento frag = parsearUnion(ep);
        if (ep.actual() != ')') {
            throw std::invalid_argument("se esperaba ')' en la expresion regular");
        }
        ep.avanzar();
        return frag;
    }
    char c = ep.actual();
    if (!esCaracterValido(c)) {
        throw std::invalid_argument("caracter inesperado en la expresion regular");
    }
    ep.avanzar();
    return simbolo(ep, c);
}

// kleene = atomo seguido de cero o mas '*'
Fragmento parsearKleene(EstadoParser& ep) {
    Fragmento frag = parsearAtomo(ep);
    while (ep.actual() == '*') {
        ep.avanzar();
        frag = estrella(ep, frag);
    }
    return frag;
}

// concat = uno o mas kleene seguidos, sin operador explicito entre ellos
Fragmento parsearConcat(EstadoParser& ep) {
    Fragmento frag = parsearKleene(ep);
    while (esInicioDeAtomo(ep.actual())) {
        Fragmento siguiente = parsearKleene(ep);
        frag = concatenar(ep, frag, siguiente);
    }
    return frag;
}

// union = concat separados por '|', es la regla de mas bajo nivel de precedencia
Fragmento parsearUnion(EstadoParser& ep) {
    Fragmento izquierda = parsearConcat(ep);
    while (ep.actual() == '|') {
        ep.avanzar();
        Fragmento derecha = parsearConcat(ep);
        izquierda = unir(ep, izquierda, derecha);
    }
    return izquierda;
}

}

// punto de entrada, arma el afn completo a partir de la regex
AFN convertirRegexAAFN(const std::string& regex) {
    if (regex.empty()) {
        throw std::invalid_argument("la expresion regular no puede estar vacia");
    }
    AFN afn;
    EstadoParser ep{regex, 0, 0, afn};
    Fragmento frag = parsearUnion(ep);
    if (ep.pos != regex.size()) {
        throw std::invalid_argument(
            "caracteres sobrantes al final de la expresion regular");
    }
    afn.inicial = frag.inicio;
    afn.finales = {frag.fin};
    return afn;
}

}