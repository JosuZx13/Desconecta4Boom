#include <iostream>
#include <cstdlib>
#include <vector>
#include <queue>
#include "player.h"
#include "environment.h"

using namespace std;

const double masinf=9999999999.0, menosinf=-9999999999.0;


// Constructor
Player::Player(int jug){
    jugador_=jug;
}

// Actualiza el estado del juego para el jugador
void Player::Perceive(const Environment & env){
    actual_=env;
}

double Puntuacion(int jugador, const Environment &estado){
    double suma=0;

    for (int i=0; i<7; i++)
      for (int j=0; j<7; j++){
         if (estado.See_Casilla(i,j)==jugador){
            if (j<3)
               suma += j;
            else
               suma += (6-j);
         }
      }

    return suma;
}


// Funcion de valoracion para testear Poda Alfabeta
double ValoracionTest(const Environment &estado, int jugador){
    int ganador = estado.RevisarTablero();

    if (ganador==jugador)
       return 99999999.0; // Gana el jugador que pide la valoracion
    else if (ganador!=0)
            return -99999999.0; // Pierde el jugador que pide la valoracion
    else if (estado.Get_Casillas_Libres()==0)
            return 0;  // Hay un empate global y se ha rellenado completamente el mesa
    else
          return Puntuacion(jugador,estado);
}

// ------------------- Los tres metodos anteriores no se pueden modificar

int EstablecerRival(int& jugador){

  if(jugador == 1){
    return 2;
  }

  return 1;

}

//Para comprobar cuantas fichas hay en una columna, avanza la FILA, manteniendo COLUMNA
double Vertical(const Environment& estado, int fil, int col, int altura, int jugador){

  int consecutivas = 0;
  double valoracion = 0;
  int rival = EstablecerRival(jugador);


  for(int f = fil; f < FILAS_COLUMNAS; f++){

    if(estado.See_Casilla(f, col) == jugador){
      consecutivas++;
    
    }else{ //Si se rompe la cadena hay que comprobar qué hay alrededor
      //Si por ejemplo, en la casilla (0,2) se rompe la secuencia, por una casilla enemiga
      //se comprueba que arriba no haya una del mismo color. Si es así y en el tablero hay una bomba enemiga, no se rompe la cadena

      if(estado.See_Casilla(f, col) == rival){

        if(f+1 < FILAS_COLUMNAS){
          
          //Si la casilla donde se rompe la cadena es del jugador, pero el rival tiene una bomba, hay que tener cuidado
          if(estado.See_Casilla(f+1, col) == jugador){

            if(estado.Have_BOOM(rival)){
              valoracion += FACTOR_BOMBAENEMIGACONSECUTIVAS;
              consecutivas++; //Se contempla como que no se rompe la cadena
            }else{
              valoracion += FACTOR_SUPERPUESTA;
              break;
            }

            
          }else{
            break;
          }
        
        }

      }else{
        break;
      }

    }

  }

  if(consecutivas >= altura){
    valoracion += (FACTOR_CONSECUTIVAS*altura);
  }

  return valoracion;

}

//Para comprobar cuantas fichas hay en una fila, avanza la COLUMNA, manteniendo FILA
double Horizontal(const Environment& estado, int fil, int col, int altura, int jugador){

  int consecutivas = 0;
  double valoracion = 0;
  int rival = EstablecerRival(jugador);


  for(int c = col; c < FILAS_COLUMNAS; c++){

    if(estado.See_Casilla(fil, c) == jugador){
      consecutivas++;
    
    }else{ //Si se rompe la cadena hay que comprobar qué hay alrededor
      //Si por ejemplo, en la casilla (0,2) se rompe la secuencia, por una casilla enemiga
      //se comprueba que arriba no haya una del mismo color. Si es así y en el tablero hay una bomba enemiga, no se rompe la cadena

      if(estado.See_Casilla(fil, c) == rival){

        if(c+1 < FILAS_COLUMNAS){
          
          //Si la casilla donde se rompe la cadena es del jugador, pero el rival tiene una bomba, hay que tener cuidado
          if(estado.See_Casilla(fil, c+1) == jugador){

            if(estado.Have_BOOM(rival)){
              valoracion += FACTOR_BOMBAENEMIGACONSECUTIVAS;
              consecutivas++; //Se contempla como que no se rompe la cadena
            }else{
              valoracion += FACTOR_SUPERPUESTA;
              break;
            }

            
          }else{
            break;
          }
        
        }

      }else{
        break;
      }

    }

  }

  if(consecutivas >= altura){
    valoracion += (FACTOR_CONSECUTIVAS*altura);
  }

  return valoracion;

}

//Para comprobar cuantas fichas hay en una diagonal ascendente, avanza COLUMNA y FILA
double DiagonalAscendente(const Environment& estado, int fil, int col, int altura, int jugador){

  int consecutivas = 0;
  double valoracion = 0;
  int rival = EstablecerRival(jugador);

  //Ascendente
  int c = col;
  for(int f = fil; f < FILAS_COLUMNAS && c < FILAS_COLUMNAS; f++, c++){

    if(estado.See_Casilla(f, c) == jugador){
      consecutivas++;
    
    }else{

      if(estado.See_Casilla(f, c) == rival){

        if(f+1 < FILAS_COLUMNAS){

          //Si la casilla donde se rompe la cadena es del jugador, pero el rival tiene una bomba, hay que tener cuidado
          if(estado.See_Casilla(f+1, c) == jugador){

            if(estado.Have_BOOM(rival)){
              valoracion += FACTOR_BOMBAENEMIGACONSECUTIVAS;
              consecutivas++; //Se contempla como que no se rompe la cadena
            }else{
              valoracion += FACTOR_SUPERPUESTA;
              break;
            }

            
          }else{
            break;
          }

        }

      }else{ //Si tampoco es del rival, no hay nada

        break;

      }

    }

  }

  if(consecutivas >= altura){
    valoracion += (FACTOR_CONSECUTIVAS*altura);
  }

  return valoracion;

}

//Para comprobar cuantas fichas hay en una diagonal descendente, disminuye FILA y avanza COLUMNA
double DiagonalDescendente(const Environment& estado, int fil, int col, int altura, int jugador){

  int consecutivas = 0;
  double valoracion = 0;
  int rival = EstablecerRival(jugador);

  //Ascendente
  int c = col;
  for(int f = fil; f >= 0 && c < FILAS_COLUMNAS; f--, c++){

    if(estado.See_Casilla(f, c) == jugador){
      consecutivas++;
    
    }else{

      if(estado.See_Casilla(f, c) == rival){

        if(f+1 < FILAS_COLUMNAS){

          //Si la casilla donde se rompe la cadena es del jugador, pero el rival tiene una bomba, hay que tener cuidado
          if(estado.See_Casilla(f+1, c) == jugador){

            if(estado.Have_BOOM(rival)){
              valoracion += FACTOR_BOMBAENEMIGACONSECUTIVAS;
              consecutivas++; //Se contempla como que no se rompe la cadena
            }else{
              valoracion += FACTOR_SUPERPUESTA;
              break;
            }

            
          }else{
            break;
          }

        }

      }else{ //Si tampoco es del rival, no hay nada

        break;

      }

    }

  }

  if(consecutivas >= altura){
    valoracion += (FACTOR_CONSECUTIVAS*altura);
  }

  return valoracion;

}

int switchAnalizador(const Environment &estado, int fil, int col, int altura, int jugador, int forma){

  int consecutivas = 0;
  int rupturaConsecutivas = false;
  int valoracion = 0;

  switch(forma){

    //VERTICAL |
    case 1:

      valoracion = Vertical(estado, fil, col, altura, jugador);

    break;

    //HORIZONTAL -
    case 2:

      valoracion = Horizontal(estado, fil, col, altura, jugador);

    break;

    //DIAGONALES  
    case 3:
      
      valoracion = DiagonalAscendente(estado, fil, col, altura, jugador) + DiagonalDescendente(estado, fil, col, altura, jugador);
      

    break;

  }

  return valoracion; //Valores Negativos

}

/**
 * Comprueba si hay X fichas consecutivas acorte a la altura recibida.
 * @param estado, la situación del juego actual
 * @param fil, la fila en la que comienza la búsqueda
 * @param col, la columna en la que comienza la búsqueda. Con ambas variables se situa en el plano
 * @param altura, el maximo de consecutivas que se quiere comprobar
 * @param jugador, el jugador del que se quieren ver sus fichas
 * @param forma, es un entero que indica si se quieren comprobar las consecutivas verticales (1), horizontales(2) o la diagonal(3)
 * @return int, 1 si las consecutivas es mayor al maximo permitido (altura), 0 si no es así
 */
double comprobarConsecutivos(const Environment &estado, int fil, int col, int altura, int jugador, int forma){

  return switchAnalizador(estado, fil, col, altura, jugador, forma); //Valores negativos

}

/**
 * Metodo que da una valoracion a la mesa actual calculando el número de fichas consecutivs del jugador
 * @param estado, es la situacion actual de juego
 * @param jugador, es el jugador que será evaluado (1 VERDE, 2 AZUL)
 * @param altura, el nivel máximo de fichas consecutivas que se quiere analizar
 * @param bomba, TRUE si el valor heurístico es gracias a usar una bomba, FALSE si no lo es
 */
double ValorConsecutivas(const Environment &estado, int jugador, int altura) {

	double valor = 0;
  bool derrota = false;

	for (int f = 0; f < FILAS_COLUMNAS; f++) { 
	
  	for (int c = 0; c < FILAS_COLUMNAS; c++) { 

			//Antes de que ocurra esto no merece la pena valorar nada
			if (estado.See_Casilla(f, c) == jugador) {
				
        //Valoracion Vertical
        valor += comprobarConsecutivos(estado, f, c, altura, jugador, 1); //VALE 0-1
				
				//Valoracion Horizontal
				valor += comprobarConsecutivos(estado, f, c, altura, jugador, 2); //VALE 0-1
				
				//Valoracion de las Diagonales
				valor += comprobarConsecutivos(estado, f, c, altura, jugador, 3); //VALE 0-2
			}

		}
	}

	// Devolvemos la suma de la valoracion de las casillas consecutivas encontrados para la altura deseada a comprobar
	return valor;

}

//See_Casilla(int row, int col);


//Jugador 1 == Verde
//Jugador 2 == Azul
// Funcion heuristica (ESTA ES LA QUE TENEIS QUE MODIFICAR)
/**
  *Funcion Heurística para valorar un nodo al usar el algoritmo PodaAlfaBeta
  * @param estado, es la situación actual del tablero de juego
  * @param miJugador, es el entero que representa a mi jugador en la partida actual (1 VERDE, 2 AZUL)
  * @return double, que es la valoracion que se la da a ese estado
  */
double Valoracion(const Environment &estado, const int& miJugador){
  double resultadoHeuristico = 0;
  double resultadoRival = 0;
  double valorHeuristico = 0;

  // Obtenemos el judador opuesto
	int rival = 1;
	if (miJugador == 1) {
		rival = 2;
  }

  if(estado.JuegoTerminado()){

    if(estado.RevisarTablero2() == miJugador){
      return FACTOR_VICTORIA;
    }else{
      return FACTOR_DERROTA;
    }

  }else{ //Supongamos que podemos usar una bomba y al usarla, se gana

    if(estado.Have_BOOM(miJugador)){
      //Si la tiene, se puede generar un nextMove(BOMBA);
      int accionBomba = 6; //En realidad es el PUT[7]
      Environment simulado = estado.GenerateNextMove(accionBomba);

      if(simulado.JuegoTerminado()){
        
        if(simulado.RevisarTablero2() == miJugador){
          return FACTOR_VICTORIA;
        
        }else{
          return FACTOR_DERROTA;
        }
      }
    }
  }

	double altura4 = ValorConsecutivas(estado, miJugador, 4); //Ha perdido
	double altura3 = ValorConsecutivas(estado, miJugador, 3); //3 consecutivas
	double altura2 = ValorConsecutivas(estado, miJugador, 2); //2 consecutivas

  double rival4 = ValorConsecutivas(estado, rival, 4);
  double rival3 = ValorConsecutivas(estado, rival, 3);
  double rival2 = ValorConsecutivas(estado, rival, 2);

  
	// Calculamos el valor heurístico de la mesa
  resultadoHeuristico = (altura4*FACTOR4_JUGADOR + altura3*FACTOR3 + altura2*FACTOR2_JUGADOR);
  resultadoRival = (rival4*FACTOR4_RIVAL + rival3*FACTOR3 + rival2*FACTOR2_RIVAL);

  valorHeuristico = (resultadoHeuristico - resultadoRival);

	return valorHeuristico;

}

// Esta funcion no se puede usar en la version entregable
// Aparece aqui solo para ILUSTRAR el comportamiento del juego
// ESTO NO IMPLEMENTA NI MINIMAX, NI PODA ALFABETA
void JuegoAleatorio(bool aplicables[], int opciones[], int &j){
    j=0;
    for (int i=0; i<8; i++){
        if (aplicables[i]){
           opciones[j]=i;
           j++;
        }
    }
}

// Invoca el siguiente movimiento del jugador
Environment::ActionType Player::Think(){
    const int PROFUNDIDAD_MINIMAX = 6;  // Umbral maximo de profundidad para el metodo MiniMax
    const int PROFUNDIDAD_ALFABETA = 8; // Umbral maximo de profundidad para la poda Alfa_Beta

    Environment::ActionType accion; // acci�n que se va a devolver
    bool aplicables[8]; // Vector bool usado para obtener las acciones que son aplicables en el estado actual. La interpretacion es
                        // aplicables[0]==true si PUT1 es aplicable
                        // aplicables[1]==true si PUT2 es aplicable
                        // aplicables[2]==true si PUT3 es aplicable
                        // aplicables[3]==true si PUT4 es aplicable
                        // aplicables[4]==true si PUT5 es aplicable
                        // aplicables[5]==true si PUT6 es aplicable
                        // aplicables[6]==true si PUT7 es aplicable
                        // aplicables[7]==true si BOOM es aplicable
    
    double valor; // Almacena el valor con el que se etiqueta el estado tras el proceso de busqueda.
    double alpha, beta; // Cotas de la poda AlfaBeta
    
    int n_act; //Acciones posibles en el estado actual

    n_act = actual_.possible_actions(aplicables); // Obtengo las acciones aplicables al estado actual en "aplicables"
    int opciones[10];

    // Muestra por la consola las acciones aplicable para el jugador activo
    //actual_.Pintamesa();
    cout << " Acciones aplicables ";
    (jugador_==1) ? cout << "Verde: " : cout << "Azul: ";
    for (int t=0; t<8; t++)
      if (aplicables[t])
         cout << " " << actual_.ActionStr( static_cast< Environment::ActionType > (t)  );
    cout << endl;

/*
    //--------------------- COMENTAR Desde aqui
    cout << "\n\t";
    int n_opciones=0;
    JuegoAleatorio(aplicables, opciones, n_opciones);

    if (n_act==0){
      (jugador_==1) ? cout << "Verde: " : cout << "Azul: ";
      cout << " No puede realizar ninguna accion!!!\n";
      //accion = Environment::actIDLE;
    }
    else if (n_act==1){
           (jugador_==1) ? cout << "Verde: " : cout << "Azul: ";
            cout << " Solo se puede realizar la accion "
                 << actual_.ActionStr( static_cast< Environment::ActionType > (opciones[0])  ) << endl;
            accion = static_cast< Environment::ActionType > (opciones[0]);

         }
         else { // Hay que elegir entre varias posibles acciones
            int aleatorio = rand()%n_opciones;
            cout << " -> " << actual_.ActionStr( static_cast< Environment::ActionType > (opciones[aleatorio])  ) << endl;
            accion = static_cast< Environment::ActionType > (opciones[aleatorio]);
         }

    //--------------------- COMENTAR Hasta aqui
*/
    


    //--------------------- AQUI EMPIEZA LA PARTE A REALIZAR POR EL ALUMNO ------------------------------------------------

    //valor = Poda_AlfaBeta(actual_, jugador_, 0, PROFUNDIDAD_ALFABETA, accion, alpha, beta);
    //cout << "Valor MiniMax: " << valor << "  Accion: " << actual_.ActionStr(accion) << endl;

    // Opcion: Poda AlfaBeta
    // NOTA: La parametrizacion es solo orientativa
    // valor = Poda_AlfaBeta(actual_, jugador_, 0, PROFUNDIDAD_ALFABETA, accion, alpha, beta);
    //cout << "Valor MiniMax: " << valor << "  Accion: " << actual_.ActionStr(accion) << endl;
    //accion = Environment::PUT1;
    
    alpha = menosinf; // Inicializar al minimo. Cualquier valor será mejor
    beta = masinf; //Se inicia al máximo. Cualquier valor será peor   
    
    //PROFUNDIDAD_MINIMAX - PROFUNDIDAD_ALFABETA
    int prof_maxima = PROFUNDIDAD_ALFABETA;
    
    //valor = MiniMax(actual_, jugador_, 0, prof_maxima, accion);
    valor = AlfaBeta(actual_, jugador_, jugador_, 0, prof_maxima, accion, alpha, beta);
    
    return accion;
}


//ALGORITMOS

/*
PSEUDOCÓDIGO PODA ALFA-BETA

funcion alfabeta(estado, jugador, prof, prof_maxima, accion, alfa, beta)
  si estado es TERMINAL o prof=0
    devolver evaluar(estado)
  
  si jugador==1
    para cada hijo de nodo
      alfa = max(alfa, alfabeta(hijo, prof-1, alfa, beta, jugador2))
      si beta <= alfa
        romper (break)
      fsi
    devolver alfa
    fpara
  
  si no (jugador==2 entonces)
    para cada hijo de nodo
      beta = min(beta, alfabeta(hijo, prof-1, alfa, beta, jugador1))

      si beta <= alfa
        romper (break)
      fsi
    devolver beta
    fpara
  fsi
end
*/

bool maximizar(double& alfa, double aux){

  if(aux > alfa){
    alfa = aux;
    return true;
  }

  return false;

}

bool minimizar(double& beta, double aux){

  if(aux < beta){
    beta = aux;
    return true;
  }

  return false;

}

//PODA ALFA-BETA
double Player::AlfaBeta(const Environment & mesa, int jugador, const int& miJugador, int prof, const int& prof_alfabeta, Environment::ActionType & accion, double alfa, double beta) {

  Environment::ActionType accion_anterior;
  
	bool opciones[8];

	int n_act = mesa.possible_actions(opciones);

	if (prof==prof_alfabeta || n_act==0 || mesa.JuegoTerminado()){ // Hemos llegado al nodo final o no hay mas acciones

		return Valoracion(mesa, miJugador);

	} else { //Hijos

    int ult_act = -1;
    Environment hijo = mesa.GenerateNextMove(ult_act);

    if(jugador == miJugador){ //NODO MAX
      
      while(!(mesa==hijo)){
                
        if(maximizar(alfa, AlfaBeta(hijo, hijo.JugadorActivo(), miJugador, prof+1, prof_alfabeta, accion_anterior, alfa, beta))){
          accion = static_cast <Environment::ActionType > (ult_act); //La mejor accion por ahora
        }

        if(beta <= alfa){
          break;
        }

        hijo = mesa.GenerateNextMove(ult_act);

      }

      return alfa;
    
    }else{ //NODO MIN

      while(!(mesa==hijo)){
        
        if(minimizar(beta, AlfaBeta(hijo, hijo.JugadorActivo(), miJugador, prof+1, prof_alfabeta, accion_anterior, alfa, beta))){
          accion = static_cast <Environment::ActionType > (ult_act); //La que menos perdidas genera
        }

        if(beta <= alfa){
          break;
        }

        hijo = mesa.GenerateNextMove(ult_act);

      }

      return beta;
      
    }

	}

}//FIN IMPLEMENTACION
