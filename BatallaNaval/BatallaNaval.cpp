#include <iostream>
#include <fstream>
using namespace std;

// estructuras para batallaNaval


struct Jugador {
	char nombre[30];
	char usuario[20];
	char contrasena[20];
	int id;
	int victorias;
	int derrotas;
};

struct Tablero {
	int duenoId;
	char oceanoPropio[10][10];
	char oceanoOponente[10][10];
	int barcosRestantes;
	int vidasPorBarco[6];
};

struct Partida {
	Jugador jugador1;
	Jugador jugador2;
	Tablero tablero1;
	Tablero tablero2;
	int turnoActual; // 1 o 2
	int finalizada;  // 0 o 1
	int ganador;     // 0 = nadie, 1 o 2
};

// Variables globales
Jugador jugadores[50];
int totalJugadores = 0;
Partida partida;
int indiceJugador1 = -1;
int indiceJugador2 = -1;
int juegoTerminado = 0;

char nombresPosiciones[10][20] = {
	"Posiciones1.txt",
	"Posiciones2.txt",
	"Posiciones3.txt",
	"Posiciones4.txt",
	"Posiciones5.txt",
	"Posiciones6.txt",
	"Posiciones7.txt",
	"Posiciones8.txt",
	"Posiciones9.txt",
	"Posiciones10.txt"
};

// Prototipos
int sonIguales(char a[], char b[]);
int cargarJugadoresDesdeArchivo();
int usuarioDisponible(char usuario[]);
int registrarJugador();
int buscarJugadorPorUsuarioYContrasena(char usuario[], char contrasena[]);
double porcentajeVictorias(int victorias, int derrotas);
void mostrarBienvenida(Jugador j);
void sobrescribirArchivoJugadores();
void inicializarTableros();
void cargarDistribucionDesdeArchivo(int numeroArchivo, int numeroTablero);
void mostrarTablerosDeJugador(int numJugador);
int validarDentroDeTablero(int fila, int columna);
int casillaYaDisparada(int numAtacante, int fila, int columna);
int aplicarDisparo(int numAtacante, int fila, int columna);
int cambiarTurnoSiFalla(int turnoActual, int resultadoDisparo);
void prepararPartida();
void jugarPartida();
void realizarTurno(int numAtacante);

// main pa mostrar en la consola
int main() {
	totalJugadores = cargarJugadoresDesdeArchivo();

	int opcionInicial = 0;

	do {
		cout << "===== MENU PRINCIPAL =====" << endl;
		cout << "1. Iniciar sesion" << endl;
		cout << "2. Registrar nuevo jugador" << endl;
		cout << "3. Salir" << endl;
		cout << "Seleccione una opcion: " << endl;
		cin >> opcionInicial;

		if (opcionInicial == 1) {
			char usuario[20];
			char clave[20];

			cout << "Usuario: " << endl;
			cin >> usuario;
			cout << "Clave (solo numeros): " << endl;
			cin >> clave;

			int indice = buscarJugadorPorUsuarioYContrasena(usuario, clave);
			if (indice == -1) {
				cout << "Credenciales incorrectas." << endl;
			}
			else {
				indiceJugador1 = indice;
				partida.jugador1 = jugadores[indiceJugador1];
				mostrarBienvenida(partida.jugador1);

				int opcionJ1 = 0;
				cout << "1. Iniciar juego" << endl;
				cout << "2. Salir" << endl;
				cout << "Seleccione una opcion: " << endl;
				cin >> opcionJ1;

				if (opcionJ1 == 1) {
					char usuario2[20];
					char clave2[20];

					cout << "=== Ingreso Jugador 2 ===" << endl;
					cout << "Usuario: " << endl;
					cin >> usuario2;
					cout << "Clave (solo numeros): " << endl;
					cin >> clave2;

					int indice2 = buscarJugadorPorUsuarioYContrasena(usuario2, clave2);
					if (indice2 == -1) {
						cout << "Credenciales incorrectas para Jugador 2." << endl;
					}
					else if (indice2 == indiceJugador1) {
						cout << "El jugador 2 no puede ser el mismo que el jugador 1." << endl;
					}
					else {
						indiceJugador2 = indice2;
						partida.jugador2 = jugadores[indiceJugador2];

						int numeroArchivoJugador1 = 0;
						int numeroArchivoJugador2 = 0;

						cout << "Jugador 1 (" << partida.jugador1.nombre
							<< ") seleccione distribucion de barcos (1 a 10): " << endl;
						cin >> numeroArchivoJugador1;

						cout << "Jugador 2 (" << partida.jugador2.nombre
							<< ") seleccione distribucion de barcos (1 a 10): " << endl;
						cin >> numeroArchivoJugador2;
						if (numeroArchivoJugador1 < 1 || numeroArchivoJugador1 > 10 ||
							numeroArchivoJugador2 < 1 || numeroArchivoJugador2 > 10) {

							cout << "Numero de archivo invalido. Deben estar en el rango de 1 a 10." << endl;
						}
						else {
							inicializarTableros();
							cargarDistribucionDesdeArchivo(numeroArchivoJugador1, 1);
							cargarDistribucionDesdeArchivo(numeroArchivoJugador2, 2);
							prepararPartida();
							jugarPartida();
						}
					}
				}
			}
		}
		else if (opcionInicial == 2) {
			int indiceNuevo = registrarJugador();
			if (indiceNuevo != -1) {
				cout << "Jugador registrado con exito." << endl;
			}
		}
		else if (opcionInicial == 3) {
			cout << "Saliendo..." << endl;
		}
		else {
			cout << "Opcion invalida." << endl;
		}

	} while (opcionInicial != 3);

	return 0;
}

// Funciones de texto
int sonIguales(char a[], char b[]) {
	int i = 0;
	while (a[i] != 0 || b[i] != 0) {
		if (a[i] != b[i]) {
			return 0;
		}
		i = i + 1;
	}
	return 1;
}

// Manejo de jugadores
int cargarJugadoresDesdeArchivo() {
	ifstream arch("Jugadores.txt");
	if (!arch.is_open()) {
		cout << "No se pudo abrir Jugadores.txt. Se iniciara sin jugadores." << endl;
		return 0;
	}

	int i = 0;
	while (i < 50 &&
		arch >> jugadores[i].id
		>> jugadores[i].usuario
		>> jugadores[i].contrasena
		>> jugadores[i].nombre
		>> jugadores[i].victorias
		>> jugadores[i].derrotas) {
		i = i + 1;
	}

	arch.close();
	return i;
}

int usuarioDisponible(char usuario[]) {
	int i = 0;
	while (i < totalJugadores) {
		if (sonIguales(usuario, jugadores[i].usuario) == 1) {
			return 0;
		}
		i = i + 1;
	}
	return 1;
}

int registrarJugador() {
	if (totalJugadores >= 50) {
		cout << "No se pueden registrar mas jugadores." << endl;
		return -1;
	}

	Jugador j;

	cout << "Ingrese nombre (sin espacios, solo letras): " << endl;
	cin >> j.nombre;

	cout << "Ingrese usuario (sin espacios, sin signos): " << endl;
	cin >> j.usuario;

	if (usuarioDisponible(j.usuario) == 0) {
		cout << "El usuario ya existe." << endl;
		return -1;
	}

	cout << "Ingrese contrasena (solo numeros, sin espacios): " << endl;
	cin >> j.contrasena;

	j.id = totalJugadores + 1;
	j.victorias = 0;
	j.derrotas = 0;

	ofstream arch("Jugadores.txt", ios::app);
	if (!arch.is_open()) {
		cout << "No se pudo abrir Jugadores.txt para escribir." << endl;
		return -1;
	}

	arch << j.id << " "
		<< j.usuario << " "
		<< j.contrasena << " "
		<< j.nombre << " "
		<< j.victorias << " "
		<< j.derrotas << endl;

	arch.close();

	jugadores[totalJugadores] = j;
	totalJugadores = totalJugadores + 1;

	return totalJugadores - 1;
}

int buscarJugadorPorUsuarioYContrasena(char usuario[], char contrasena[]) {
	int i = 0;
	while (i < totalJugadores) {
		if (sonIguales(usuario, jugadores[i].usuario) == 1 &&
			sonIguales(contrasena, jugadores[i].contrasena) == 1) {
			return i;
		}
		i = i + 1;
	}
	return -1;
}

double porcentajeVictorias(int victorias, int derrotas) {
	int total = victorias + derrotas;
	if (total == 0) {
		return 0.0;
	}
	double p = (victorias * 100.0) / total;
	return p;
}

void mostrarBienvenida(Jugador j) {
	cout << "BIENVENIDO." << endl;
	cout << "Nombre: " << j.nombre << endl;
	cout << "ID: " << j.id << endl;
	double resultPorcentaje = porcentajeVictorias(j.victorias, j.derrotas);
	cout << "Porcentaje de victorias: " << resultPorcentaje << "%" << endl;
}

void sobrescribirArchivoJugadores() {
	ofstream arch("Jugadores.txt");
	if (!arch.is_open()) {
		cout << "No se pudo abrir Jugadores.txt para sobrescribir." << endl;
		return;
	}

	int i = 0;
	while (i < totalJugadores) {
		arch << jugadores[i].id << " "
			<< jugadores[i].usuario << " "
			<< jugadores[i].contrasena << " "
			<< jugadores[i].nombre << " "
			<< jugadores[i].victorias << " "
			<< jugadores[i].derrotas << endl;
		i = i + 1;
	}

	arch.close();
}

// Tableros y partida
void inicializarTableros() {
	int i = 0;
	while (i < 10) {
		int j = 0;
		while (j < 10) {
			partida.tablero1.oceanoPropio[i][j] = '.';
			partida.tablero1.oceanoOponente[i][j] = '.';
			partida.tablero2.oceanoPropio[i][j] = '.';
			partida.tablero2.oceanoOponente[i][j] = '.';
			j = j + 1;
		}
		i = i + 1;
	}

	int k = 0;
	while (k < 6) {
		partida.tablero1.vidasPorBarco[k] = 0;
		partida.tablero2.vidasPorBarco[k] = 0;
		k = k + 1;
	}
	partida.tablero1.barcosRestantes = 6;
	partida.tablero2.barcosRestantes = 6;

	partida.tablero1.duenoId = partida.jugador1.id;
	partida.tablero2.duenoId = partida.jugador2.id;
}

void cargarDistribucionDesdeArchivo(int numeroArchivo, int numeroTablero) {
	if (numeroArchivo < 1 || numeroArchivo > 10) {
		cout << "Numero de archivo de posiciones invalido." << endl;
		return;
	}

	ifstream arch(nombresPosiciones[numeroArchivo - 1]);
	if (!arch.is_open()) {
		cout << "No se pudo abrir archivo de posiciones." << endl;
		return;
	}

	int i = 0;
	while (i < 10) {
		int j = 0;
		while (j < 10) {
			char c;
			arch >> c;
			if (!arch) {
				c = '.';
			}

			if (numeroTablero == 1) {
				partida.tablero1.oceanoPropio[i][j] = c;
				if (c >= '1' && c <= '6') {
					int indiceBarco = (int)(c - '1');
					if (indiceBarco >= 0 && indiceBarco < 6) {
						partida.tablero1.vidasPorBarco[indiceBarco] =
							partida.tablero1.vidasPorBarco[indiceBarco] + 1;
					}
				}
			}
			else {
				partida.tablero2.oceanoPropio[i][j] = c;
				if (c >= '1' && c <= '6') {
					int indiceBarco = (int)(c - '1');
					if (indiceBarco >= 0 && indiceBarco < 6) {
						partida.tablero2.vidasPorBarco[indiceBarco] =
							partida.tablero2.vidasPorBarco[indiceBarco] + 1;
					}
				}
			}

			j = j + 1;
		}
		i = i + 1;
	}

	arch.close();
}

void mostrarTablerosDeJugador(int numJugador) {
	char columnas[10] = { 'A','B','C','D','E','F','G','H','I','J' };

	cout << "Tablero oponente (disparos realizados):" << endl;

	cout << "   ";
	int c = 0;
	while (c < 10) {
		cout << columnas[c] << " ";
		c = c + 1;
	}
	cout << endl;

	int i = 0;
	while (i < 10) {
		cout << " " << i << " ";

		int j = 0;
		while (j < 10) {
			if (numJugador == 1) {
				cout << partida.tablero1.oceanoOponente[i][j] << " ";
			}
			else {
				cout << partida.tablero2.oceanoOponente[i][j] << " ";
			}
			j = j + 1;
		}
		cout << endl;
		i = i + 1;
	}
}

// Reglas de disparo
int validarDentroDeTablero(int fila, int columna) {
	if (fila < 0 || fila > 9) {
		return 0;
	}
	if (columna < 0 || columna > 9) {
		return 0;
	}
	return 1;
}

int casillaYaDisparada(int numAtacante, int fila, int columna) {
	if (numAtacante == 1) {
		char c = partida.tablero1.oceanoOponente[fila][columna];
		if (c == '*' || c == 'X') {
			return 1;
		}
	}
	else {
		char c = partida.tablero2.oceanoOponente[fila][columna];
		if (c == '*' || c == 'X') {
			return 1;
		}
	}
	return 0;
}

// 0 es para el fallo
// 1 es para el acierto
// 2 es para el acierto y gana
int aplicarDisparo(int numAtacante, int fila, int columna) {
	if (numAtacante == 1) {
		char c = partida.tablero2.oceanoPropio[fila][columna];
		if (c == '.') {
			partida.tablero1.oceanoOponente[fila][columna] = '*';
			cout << "Fallo." << endl;
			return 0;
		}
		else if (c >= '1' && c <= '6') {
			partida.tablero1.oceanoOponente[fila][columna] = 'X';
			partida.tablero2.oceanoPropio[fila][columna] = 'X';

			int indiceBarco = (int)(c - '1');
			partida.tablero2.vidasPorBarco[indiceBarco] =
				partida.tablero2.vidasPorBarco[indiceBarco] - 1;

			cout << "Acierto!" << endl;

			if (partida.tablero2.vidasPorBarco[indiceBarco] == 0) {
				partida.tablero2.barcosRestantes =
					partida.tablero2.barcosRestantes - 1;
				cout << "Hundiste un barco." << endl;
			}

			if (partida.tablero2.barcosRestantes == 0) {
				return 2;
			}

			return 1;
		}
		else {
			partida.tablero1.oceanoOponente[fila][columna] = '*';
			cout << "Fallo." << endl;
			return 0;
		}
	}
	else {
		char c = partida.tablero1.oceanoPropio[fila][columna];
		if (c == '.') {
			partida.tablero2.oceanoOponente[fila][columna] = '*';
			cout << "Fallo." << endl;
			return 0;
		}
		else if (c >= '1' && c <= '6') {
			partida.tablero2.oceanoOponente[fila][columna] = 'X';
			partida.tablero1.oceanoPropio[fila][columna] = 'X';

			int indiceBarco = (int)(c - '1');
			partida.tablero1.vidasPorBarco[indiceBarco] =
				partida.tablero1.vidasPorBarco[indiceBarco] - 1;

			cout << "Acierto!" << endl;

			if (partida.tablero1.vidasPorBarco[indiceBarco] == 0) {
				partida.tablero1.barcosRestantes =
					partida.tablero1.barcosRestantes - 1;
				cout << "Hundiste un barco." << endl;
			}

			if (partida.tablero1.barcosRestantes == 0) {
				return 2;
			}

			return 1;
		}
		else {
			partida.tablero2.oceanoOponente[fila][columna] = '*';
			cout << "Fallo." << endl;
			return 0;
		}
	}
}

int cambiarTurnoSiFalla(int turnoActual, int resultadoDisparo) {
	if (resultadoDisparo == 0) {
		if (turnoActual == 1) {
			return 2;
		}
		else {
			return 1;
		}
	}
	return turnoActual;
}

// Flujo de partida
void prepararPartida() {
	partida.turnoActual = 2; // inicia jugador 2
	partida.finalizada = 0;
	partida.ganador = 0;
}

void jugarPartida() {
	juegoTerminado = 0;

	while (juegoTerminado == 0) {
		realizarTurno(partida.turnoActual);
	}
}

void realizarTurno(int numAtacante) {
	int fila;
	int columna;
	char columnaChar;

	if (numAtacante == 1) {
		cout << "===== Turno del Jugador 1 =====" << endl;
	}
	else {
		cout << "===== Turno del Jugador 2 =====" << endl;
	}

	mostrarTablerosDeJugador(numAtacante);

	int coordenadaValida = 0;

	while (coordenadaValida == 0 && juegoTerminado == 0) {
		cout << "Ingrese columna (A-J) y fila (0-9)." << endl;
		cout << "Para rendirse escriba Z 0." << endl;
		cin >> columnaChar >> fila;

		if (columnaChar == 'Z' && fila == 0) {
			cout << "El jugador" << numAtacante << "se rinde." << endl;
			partida.finalizada = 1;
			juegoTerminado = 1;
			if (numAtacante == 1) {
				partida.ganador = 2;
				jugadores[indiceJugador2].victorias =
					jugadores[indiceJugador2].victorias + 1;
				jugadores[indiceJugador1].derrotas =
					jugadores[indiceJugador1].derrotas + 1;
			}
			else {
				partida.ganador = 1;
				jugadores[indiceJugador1].victorias =
					jugadores[indiceJugador1].victorias + 1;
				jugadores[indiceJugador2].derrotas =
					jugadores[indiceJugador2].derrotas + 1;
			}
			sobrescribirArchivoJugadores();
			return;
		}

		columna = (int)(columnaChar - 'A');

		if (validarDentroDeTablero(fila, columna) == 0) {
			cout << "Coordenada fuera del tablero. Intente de nuevo." << endl;
		}
		else if (casillaYaDisparada(numAtacante, fila, columna) == 1) {
			cout << "Ya disparaste a esa casilla. Intente de nuevo." << endl;
		}
		else {
			coordenadaValida = 1;
		}
	}

	if (juegoTerminado == 1) {
		return;
	}

	int resultado = aplicarDisparo(numAtacante, fila, columna);

	if (resultado == 2) {
		cout << "Todos los barcos del oponente han sido hundidos." << endl;
		partida.finalizada = 1;
		juegoTerminado = 1;
		if (numAtacante == 1) {
			partida.ganador = 1;
			jugadores[indiceJugador1].victorias =
				jugadores[indiceJugador1].victorias + 1;
			jugadores[indiceJugador2].derrotas =
				jugadores[indiceJugador2].derrotas + 1;
		}
		else {
			partida.ganador = 2;
			jugadores[indiceJugador2].victorias =
				jugadores[indiceJugador2].victorias + 1;
			jugadores[indiceJugador1].derrotas =
				jugadores[indiceJugador1].derrotas + 1;
		}
		sobrescribirArchivoJugadores();
		return;
	}

	partida.turnoActual = cambiarTurnoSiFalla(partida.turnoActual, resultado);
}
