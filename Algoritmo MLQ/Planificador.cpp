#include "Planificador.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>

// Método para leer procesos desde un archivo
void Planificador::leerProcesos(const string& filename) {
    ifstream file(filename);
    string line;
    
    // Saltar líneas de encabezado
    getline(file, line);
    getline(file, line);
    
    while (getline(file, line)) {
        stringstream ss(line);
        string label;               // Label se refiere a la etiqueta del Proceso
        int bt, at, q, prioridad;   
        
        getline(ss, label, ';');    // Leer etiqueta
        ss >> bt;                   // Leer burstTime
        ss.ignore(1);               // Ignorar ';'
        ss >> at;                   // Leer arrivalTime
        ss.ignore(1);
        ss >> q;                    // Leer cola
        ss.ignore(1);
        ss >> prioridad;            // Leer prioridad

        // Creo el objeto proceso con los datos leídos del .txt
        Proceso p(label, bt, at, q, prioridad); 
        
        // Agregar el proceso al vector
        procesos.push_back(p);  
        
        // Colocar el proceso en la cola correspondiente
        // Q1(RR(3)) ; Q2(RR(5)) ; Q3(FCFS)
        if (q == 1) q1.push(p);
        else if (q == 2) q2.push(p);
        else if (q == 3) q3.push(p);
    }
}


// Método para planificar y ejecutar procesos según el esquema MLQ
// MLQ :RR(3), RR(5), FCFS.
void Planificador::programarProcesos() {
    int currentTime = 0; // Comenzamos desde el tiempo 0

    // Mientras haya procesos en alguna de las colas, el programa sigue su curso
    while (!q1.empty() || !q2.empty() || !q3.empty()) {
        
        // Empieza a procesar la Cola 1 (RR(3))
        while (!q1.empty() && q1.front().arrivalTime <= currentTime) {
            Proceso p = q1.front(); // Copio el primer proceso de la cola
            q1.pop();               // Elimino el original de la cola

            // Si es la primera vez que se está ejecutando este proceso, se guarda el tiempo actual como su tiempo de respuesta
            if (p.responseTime == -1) {
                p.responseTime = currentTime; 
            }
            
            // Se toma el quantum de tiempo para trabajar en el proceso (3)
            int timeSlice = min(3, p.burstTime);
            p.burstTime -= timeSlice; // Se resta el tiempo que se ejecutó
            currentTime += timeSlice; // Avanza el tiempo actual

            // Si aún queda tiempo en el proceso, este vuelve a entrar en la cola
            if (p.burstTime > 0) {
                q1.push(p); // El proceso se reinserta si no ha terminado
            } else { // Si ya terminó
                p.completionTime = currentTime; // Se guarda su CT
                p.turnaroundTime = p.completionTime - p.arrivalTime; // Se calcula su TAT
                p.waitingTime = p.turnaroundTime - p.originalBurstTime; // Se calcula el WT
                procesos.push_back(p); // Añado el proceso que terminó a la lista de procesos
            }
        }

        // Empieza a procesar la Cola 2 (RR(5))
        while (!q2.empty() && q2.front().arrivalTime <= currentTime) {
            Proceso p = q2.front(); // Copio el primer proceso de la cola
            q2.pop();               // Elimino el original de la cola

            // Guardo el tiempo de respuesta si es la primera vez que se ejecuta
            if (p.responseTime == -1) {
                p.responseTime = currentTime; 
            }

            // Se toma el quantum de tiempo para trabajar en el proceso (5)
            int timeSlice = min(5, p.burstTime);
            p.burstTime -= timeSlice; // Se resta el tiempo que se ejecutó
            currentTime += timeSlice; // Avanza el tiempo actual

            // Si aún queda tiempo en el proceso, este vuelve a entrar en la cola
            if (p.burstTime > 0) {
                q2.push(p); // El proceso se reinserta si no ha terminado
            } else { // Si ya terminó
                p.completionTime = currentTime; // Se guarda su CT
                p.turnaroundTime = p.completionTime - p.arrivalTime; // Se calcula su TAT
                p.waitingTime = p.turnaroundTime - p.originalBurstTime; // Se calcula el WT
                procesos.push_back(p); // Añado el proceso que terminó a la lista de procesos
            }
        }

        // Empieza a procesar la Cola 3 (FCFS)
        while (!q3.empty() && q3.front().arrivalTime <= currentTime) {
            Proceso p = q3.front(); // Copio el primer proceso de la cola
            q3.pop();               // Elimino el original de la cola

            // Guardo el tiempo de respuesta si es la primera vez que se ejecuta
            if (p.responseTime == -1) {
                p.responseTime = currentTime; 
            }

            currentTime += p.burstTime;             // Aquí usamos todo el tiempo que le queda al proceso
            p.completionTime = currentTime;         // Se guarda su CT
            p.turnaroundTime = p.completionTime - p.arrivalTime;    // Se calcula su TAT
            p.waitingTime = p.turnaroundTime - p.originalBurstTime; // Se calcula el WT
            procesos.push_back(p);                  // Añado el proceso que terminó a la lista de procesos
        }

        // Si ya no hay procesos en ninguna cola avanza el tiempo
        if (q1.empty() && q2.empty() && q3.empty()) {
            currentTime++; 
        }
    }
}


// Método para escribir resultados en un archivo de texto
void Planificador::escribirResultados(const string& filename) {
    ofstream file(filename); // Se crear el archivo con el nombre dado
    file << "# etiqueta; BT; AT; Q; Pr; WT; CT; RT; TAT\n"; // Se escribe la cabecera con los nombres de las columnas

    map<string, Proceso> procesosCombinados; // Uso un mapa para guardar procesos agrupados por su etiqueta

    // Junto los procesos que tienen la misma etiqueta
    for (const auto& p : procesos) {
        if (procesosCombinados.find(p.etiqueta) == procesosCombinados.end()) {
            // Si no se ha visto la etiqueta, se agrega al mapa
            procesosCombinados[p.etiqueta] = p;
        } else {
            // Si ya existe, se fusionan sus métricas
            Proceso& existente = procesosCombinados[p.etiqueta];    // Toma el proceso ya existente
            existente.waitingTime += p.waitingTime;                 // Suma el tiempo de espera
            existente.completionTime = max(existente.completionTime, p.completionTime); // Se queda con el mayor tiempo de finalización
            existente.turnaroundTime += p.turnaroundTime;           // Suma el tiempo de respuesta total
            existente.responseTime = p.responseTime;                // Guarda el tiempo de respuesta 
        }
    }

    // Variables para calcular promedios
    double totalWaitingTime = 0;
    double totalTurnaroundTime = 0;
    double totalResponseTime = 0;
    double totalCompletionTime = 0;
    int count = 0;

    // Se escriben los procesos ya combinadps en el archivo
    for (const auto& entry : procesosCombinados) {
        const Proceso& p = entry.second;
        file << p.etiqueta << ";" << p.burstTime << ";" << p.arrivalTime << ";" 
             << p.cola << ";" << p.priority << ";" << p.waitingTime << ";" 
             << p.completionTime << ";" << p.responseTime << ";" 
             << p.turnaroundTime << "\n";

        // Se acumulan las métricas para el cálculo de promedios
        totalWaitingTime += p.waitingTime;
        totalCompletionTime += p.completionTime;
        totalTurnaroundTime += p.turnaroundTime;
        totalResponseTime += p.responseTime;
        count++; // Número de procesos en la lista
    }

    // Calcular y escribir los promedios en el archivo
    if (count > 0) { 
        file << "\n# Promedios;\n";
        file << "WT=" << (totalWaitingTime / count) << "; ";    // Promedio de tiempo de espera
        file << "CT=" << (totalCompletionTime / count) << "; "; // Promedio de tiempo de completado
        file << "RT=" << (totalResponseTime / count) << "; ";   // Promedio de tiempo de respuesta
        file << "TAT=" << (totalTurnaroundTime / count) << ";"; // Promedio de turn around time
    }
    
    file.close();   // Cierro el archivo
}



// ------------------------------------------------- I M P O R T A N T E --------------------------------------------

// Método para imprimir los procesos por consola (SOLO PARA TESTING)
/* 
- En esta función y en la anterior 'escribirResultados' se implementó map debido a que los resultados de ejecución (Metricas) estaban siendo 
  duplicados con el archivo de texto de entrada y de salida. Así que decidi almacenar las métricas de los procesos con las mismas etiquetas.
*/

// -----------------------------------------------------------------------------------------------------------------

/*
void Planificador::imprimirEnConsola() {
    map<string, Proceso> procesosCombinados; // Mapa para almacenar procesos por etiqueta

    // Fusionar procesos con la misma etiqueta
    for (const auto& p : procesos) {
        if (procesosCombinados.find(p.etiqueta) == procesosCombinados.end()) {
            // Si la etiqueta no está en el mapa, agregar el proceso
            procesosCombinados[p.etiqueta] = p;
        } else {
            // Si ya existe, fusionar métricas
            Proceso& existente = procesosCombinados[p.etiqueta];
            existente.waitingTime += p.waitingTime;
            existente.completionTime = max(existente.completionTime, p.completionTime);
            existente.turnaroundTime += p.turnaroundTime;
            existente.responseTime = p.responseTime; 
        }
    }

    cout<<"etiqueta; BT; AT; Q; Pr; WT; CT; RT; TAT\n";
    // Imprimir los procesos con las métricas fusionadas
    for (const auto& entry : procesosCombinados) {
        const Proceso& p = entry.second;
            cout << p.etiqueta << ";" << p.burstTime << ";" << p.arrivalTime << ";" 
                  << p.cola << ";" << p.priority << ";" << p.waitingTime << ";" 
                  << p.completionTime << ";" << p.responseTime << ";" 
                  << p.turnaroundTime << "\n";
                  
    }
}
*/