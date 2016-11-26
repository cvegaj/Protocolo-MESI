# Protocolo MESIS - Criba de Eratóstenes
## Tarea Programada II/IE-0521 Estructuras de Computadores Digitales II
#
### Autores:
####         Carla Vega J.    B06763
####         Lennon Núñez M.  B34943
### Profesor: 
####   José Daniel Hernández
######################################################################
#
### El contenido del trabajo consiste en la solución en lenguaje C++ para dos implementaciones: La primera implementación consiste en un simulador del comportamiento de dos niveles de Memoria Cache para dos procesadores (CPU0 y CPU1) implementando el protocolo de coherencia MESI. Cada procesador tiene un cache L1, y ambos comparten un único cahce L2. para diferentes niveles de asociatividad en lenguaje de programación de alto nivel (C++). 
### Se requiere incluir en el mismo directorio donde se ubica el programa un archivo .trace con las direcciones de memoria y datos y el tipo de solicitud (Lectura/Escritura), para el caso del simulador del protocolo MESI. Se incluye el archivo main.cpp con el código de la implementación y un Makefile para realizar la compilación y creación del ejecutable. El Makefile utiliza g++ para la compilación, en caso de querer usar otro compilador omitir el uso del Makefile y usar el código correspondiente a preferencia.
### La segunda implementación de este trabajo consta de 

#Configuración Programa de Protocolo MESI
## Estructura de los Cache
#####           - Cache L1: Mapeo Directo, bloques de 16 B, local con tamaño de 8 kB.
#####           - Cache L2: Mapeo Directo, bloques de 16 B, compartido con tamaño de 64 kB.
# 
## Parámetros de Entrada 
####   No se requiere configurar ningún parámetro para este programa, el estado inicial de los bloques es en "I", ya que al iniciar los caches todo bloque es inválido.
#
## Corriendo el Programa
    $ make -f Makefile
    $ ./cache
    
#### Se generará el ejecutable *cache*, el cuál utilizará el archivo *aligned.trace* que deberá incluir en el directorio donde se encuentra el ejecutable. Ingrese los parámetros requeridos y en pantalla se mostrará los resultados del acceso a memoria cache de cada procesador según el caso MESI.
#
## Eliminando Archivos Generados
    $ make clean
#### Elimina el ejecutable

#Configuración Programa de Criba de Eratóstenes
