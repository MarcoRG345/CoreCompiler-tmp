# CoreCompiler-tmp

implementacion de un compilador mediante practicas estructuradas

practica 3: minimizacion de un afd con el algoritmo de particiones de hopcroft

regex -> afn (thompson) -> afd (construccion de subconjuntos) -> afd podado (sin estados inalcanzables ni muertos) -> afd minimizado (hopcroft)

## estructura

- automata.h / automata.cpp: estructuras afn y afd, impresion de tablas
- operations.h / operations.cpp: move y epsilonClosure
- thompson.h / thompson.cpp: regex a afn
- converter.h / converter.cpp: afn a afd (construccion de subconjuntos)
- pruner.h / pruner.cpp: poda de estados inalcanzables y muertos, aceptaCadena
- minimizer.h / minimizer.cpp: minimizacion por particiones (hopcroft)
- main.cpp: corre las 60 pruebas (3 regex x 20 cadenas)
- pruebas/: tests unitarios por modulo

## compilar y correr

programa principal

    g++ -std=c++17 automata.cpp operations.cpp converter.cpp thompson.cpp pruner.cpp minimizer.cpp main.cpp -o practica3
    ./practica3

tests unitarios por modulo, ejemplo con minimizer

    g++ -std=c++17 automata.cpp pruner.cpp minimizer.cpp pruebas/prueba_minimizer.cpp -o prueba_minimizer
    ./prueba_minimizer

## expresiones regulares evaluadas

1. (a|b)*abb, cadenas que terminan en abb
2. (0|1)*01(0|1)*, cadenas binarias que contienen 01
3. (0|10*1)*, cadenas binarias con numero par de unos