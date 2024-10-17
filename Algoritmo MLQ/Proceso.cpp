#include "Proceso.h"

// Constructor vacío
Proceso::Proceso() 
    : etiqueta(""), burstTime(0), originalBurstTime(0), arrivalTime(0), cola(0), priority(0),
      waitingTime(0), completionTime(0), responseTime(-1), turnaroundTime(0) {}

// Constructor de la clase Proceso
Proceso::Proceso(const string& label, int bt, int at, int q, int p)
    : etiqueta(label), burstTime(bt), originalBurstTime(bt), arrivalTime(at), cola(q), priority(p),
      waitingTime(0), completionTime(0), responseTime(-1), turnaroundTime(0) {}


