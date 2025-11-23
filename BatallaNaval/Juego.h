#ifndef JUEGO_H
#define JUEGO_H

#include <iostream>
#include <fstream>
using namespace std;

// Estructuras
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
    int turnoActual;
    int finalizada;
    int ganador;
};

//Variables globales

extern Jugador jugadores[50];
extern int totalJugadores;
extern Partida partida;
extern int indiceJugador1;
extern int indiceJugador2;
extern int juegoTerminado;
extern char nombresPosiciones[10][20];

//Prototipos de funciones

// Texto
int longitudCadena(char s[]);
void copiarCadena(char destino[], char origen[]);
int sonIguales(char a[], char b[]);

// Jugadores
int cargarJugadoresDesdeArchivo();
int usuarioDisponible(char usuario[]);
int registrarJugador();
int buscarJugadorPorUsuarioYContrasena(char usuario[], char contrasena[]);
double porcentajeVictorias(int victorias, int derrotas);
void mostrarBienvenida(Jugador j);
void sobrescribirArchivoJugadores();

// Tableros
void inicializarTableros();
void cargarDistribucionDesdeArchivo(int numeroArchivo, int numeroTablero);
void mostrarTablerosDeJugador(int numJugador);

// Disparos
int validarDentroDeTablero(int fila, int columna);
int casillaYaDisparada(int numAtacante, int fila, int columna);
int aplicarDisparo(int numAtacante, int fila, int columna);
int cambiarTurnoSiFalla(int turnoActual, int resultadoDisparo);

// Flujo de partida
void prepararPartida();
void jugarPartida();
void realizarTurno(int numAtacante);

#endif
