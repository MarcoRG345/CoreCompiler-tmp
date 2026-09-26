#include "minimizer.h"

#include <algorithm>
#include <deque>
#include <map>
#include <set>
#include <stdexcept>
#include <vector>

namespace automata {

using Bloque = std::set<int>;

static bool contieneEstado(const Bloque& bloque, int estado) {
    return bloque.find(estado) != bloque.end();
}

AFD minimizarAFD(const AFD& afd) {
    if (afd.estados.empty()) {
        throw std::invalid_argument("El AFD no tiene estados");
    }

    if (afd.inicial < 0 ||
        afd.inicial >= static_cast<int>(afd.estados.size())) {
        throw std::invalid_argument("El estado inicial del AFD no es valido");
    }

    std::vector<Bloque> particiones;

    // particion inicial p = {finales, no finales}
    Bloque finales;
    Bloque noFinales;

    for (int i = 0; i < static_cast<int>(afd.estados.size()); ++i) {
        if (afd.finales.count(i)) {
            finales.insert(i);
        } else {
            noFinales.insert(i);
        }
    }

    if (!finales.empty()) {
        particiones.push_back(finales);
    }

    if (!noFinales.empty()) {
        particiones.push_back(noFinales);
    }

    // cola de trabajo w, arranca igual que la particion inicial
    std::deque<Bloque> trabajo;

    for (const Bloque& bloque : particiones) {
        trabajo.push_back(bloque);
    }

    while (!trabajo.empty()) {
        Bloque A = trabajo.front();
        trabajo.pop_front();

        for (char simbolo : afd.alfabeto) {
            // x = preimagen de a por simbolo, estados que caen en a al leer simbolo
            Bloque X;

            for (int estado = 0;
                 estado < static_cast<int>(afd.estados.size());
                 ++estado) {

                int destino = afd.destino(estado, simbolo);

                if (destino != -1 && contieneEstado(A, destino)) {
                    X.insert(estado);
                }
            }

            if (X.empty()) {
                continue;
            }

            std::vector<Bloque> nuevasParticiones;

            for (const Bloque& Y : particiones) {
                Bloque interseccion;
                Bloque diferencia;

                for (int estado : Y) {
                    if (X.count(estado)) {
                        interseccion.insert(estado);
                    } else {
                        diferencia.insert(estado);
                    }
                }

                // y no se distingue por simbolo, se conserva igual
                if (interseccion.empty() || diferencia.empty()) {
                    nuevasParticiones.push_back(Y);
                    continue;
                }

                // y se parte en dos mitades
                nuevasParticiones.push_back(interseccion);
                nuevasParticiones.push_back(diferencia);

                bool estabaEnTrabajo = false;

                for (const Bloque& W : trabajo) {
                    if (W == Y) {
                        estabaEnTrabajo = true;
                        break;
                    }
                }

                if (estabaEnTrabajo) {
                    // y ya era distinguidor, se reemplaza por sus dos mitades
                    std::deque<Bloque> nuevaCola;

                    for (const Bloque& W : trabajo) {
                        if (W == Y) {
                            nuevaCola.push_back(interseccion);
                            nuevaCola.push_back(diferencia);
                        } else {
                            nuevaCola.push_back(W);
                        }
                    }

                    trabajo = nuevaCola;
                } else {
                    // solo se encola la mitad mas chica
                    if (interseccion.size() <= diferencia.size()) {
                        trabajo.push_back(interseccion);
                    } else {
                        trabajo.push_back(diferencia);
                    }
                }
            }

            particiones = nuevasParticiones;
        }
    }

    AFD minimo;
    minimo.alfabeto = afd.alfabeto;

    // cada bloque final se vuelve un estado nuevo del afd minimizado
    std::map<int, int> estadoABloque;

    for (int i = 0; i < static_cast<int>(particiones.size()); ++i) {
        minimo.estados.push_back(particiones[i]);

        for (int estado : particiones[i]) {
            estadoABloque[estado] = i;
        }
    }

    minimo.inicial = estadoABloque.at(afd.inicial);

    // un bloque es final si contiene algun estado final original
    for (int i = 0; i < static_cast<int>(particiones.size()); ++i) {
        for (int estado : particiones[i]) {
            if (afd.finales.count(estado)) {
                minimo.finales.insert(i);
                break;
            }
        }
    }

    // un representante por bloque basta, todos son equivalentes
    for (int i = 0; i < static_cast<int>(particiones.size()); ++i) {
        int representante = *particiones[i].begin();

        for (char simbolo : minimo.alfabeto) {
            int destino = afd.destino(representante, simbolo);

            if (destino != -1) {
                minimo.agregarTransicion(
                    i,
                    simbolo,
                    estadoABloque.at(destino)
                );
            }
        }
    }

    return minimo;
}

}