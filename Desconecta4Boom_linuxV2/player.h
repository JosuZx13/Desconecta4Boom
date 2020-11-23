#ifndef PLAYER_H
#define PLAYER_H

#include "environment.h"

const int FILAS_COLUMNAS = 7;
const int FACTOR_CONSECUTIVAS = -100; //Si las hace el jugador es negativo
const int FACTOR4_RIVAL = 100;
const int FACTOR2_RIVAL = 2;
const int FACTOR3 = 5;
const int FACTOR2_JUGADOR = 100;
const int FACTOR4_JUGADOR = 2;
const int FACTOR_SUPERPUESTA = -20;
const int FACTOR_BOMBAENEMIGACONSECUTIVAS = -50; //Quiere decir que entre dos+ fichas mias hay una enemiga y en el tablero hay una bomba
const double FACTOR_VICTORIA = 9000000;
const double FACTOR_DERROTA = -9000000;

class Player{
    public:
      Player(int jug);
      Environment::ActionType Think();
      void Perceive(const Environment &env);
	    
      /**
        * Implementacion Algoritmo Poda AlfaBeta. Cuando BETA <= ALFA, se poda.
        * @param mesa, estado actual en el desarrollo del juego
        * @param jugador, entero que indica el jugador actual al que se asocia dicho estado
        * @param miJugador, siempre almacena el valor que indica qué jugador es el propio
        * @paran prof, entero que indica por qué profundidad va
        * @param prof_maxima, entero que indica la máxima profundidad a la que se avanzará
        * @param accion, es la acción que se ha tomado como la mejor opción del jugador
        * @param alpha, double que indica el valor de alfa para la poda. Su primer valor es menosInfinito
        * @param beta, double que indica el valor de beta para la poda. Su primer valor es másInfinito
        * @return double, un valor de retorno con la valoracion para el Algoritmo
        */
      double AlfaBeta(const Environment & mesa, int jugador, const int& miJugador, int prof, const int& prof_alfabeta, Environment::ActionType & accion, double alfa, double beta);

    private:
      int jugador_; //VERDE 1, AZUL 2. LA CLASE SIEMPRE TIENE EL VALOR DE TU JUGADOR
      Environment actual_;
      
      //Metodos Environment
      //char See_Casilla(int row, int col) const; //Devuelve lo que hay en el tablero
        //0 - vacia; 1 - jugador1; 2 - jugador2; 4 - fichaBomba jug1; 5 - fichaBomba jug2
      //int Get_Ocupacion(int columna); //Devuelve la ocupacion de la columna (0-6)
        //un valor entre 0 y 7, donde 0 indica vacia y 7 llena
      //int Last_Action(int jug) const; //Orrdinal ultima acción para llegar a la situacion actual
        //Acciones de la 0 a la 7, siendo 7 boom
      //Environment GenerateNextMove(int &act) const;
};
#endif
