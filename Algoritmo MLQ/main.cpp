#include "Planificador.h"

int main() {
    
    Planificador planificador;                      // Creamos un objeto 'planificador' de la clase Planificador
    planificador.leerProcesos("mlq002.txt");        // Leemos los procesos desde el archivo 'mlq001.txt' 
    planificador.programarProcesos();               // Ejecutamos la programación de los procesos
    planificador.escribirResultados("output.txt");  // Se guardan los resultados en el archivo 'output.txt'
    
    return 0; 
}
