#ifndef PLANIFICADOR_H
#define PLANIFICADOR_H

#include <vector>
#include <queue>
#include "Proceso.h"

class Planificador {
private:
	vector<Proceso> procesos;
	queue<Proceso> q1; // Cola 1 (RR(3))
	queue<Proceso> q2; // Cola 2 (RR(5))
	queue<Proceso> q3; // Cola 3 (FCFS)

public:
	void leerProcesos(const string& filename);
	void programarProcesos();
	void escribirResultados(const string& filename);
	// Test en consola
	void imprimirEnConsola();
};

#endif // PLANIFICADOR_H
