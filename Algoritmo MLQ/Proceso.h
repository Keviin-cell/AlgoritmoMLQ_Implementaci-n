#ifndef PROCESO_H
#define PROCESO_H

#include <string>

using namespace std;

class Proceso {
public:
    // Atributos del objeto Proceso
    string etiqueta; 
    int originalBurstTime;
    int burstTime;        // Tiempo de CPU necesario (BT)
    int arrivalTime;      // Tiempo de llegada (AT)
    int cola;             // Cola a la que pertenece (Q)
    int priority;         // Prioridad dentro de la cola
    int waitingTime;      // Tiempo de espera (WT)
    int completionTime;   // Tiempo de completado (CT)
    int responseTime;     // Tiempo de respuesta (RT)
    int turnaroundTime;   // Tiempo de retorno (TAT)

    Proceso();
    Proceso(const string& label, int bt, int at, int q, int p);
    
};

#endif // PROCESO_H
