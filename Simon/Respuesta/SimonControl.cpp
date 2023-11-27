#include "SimonControl.h"
#include <iostream>


SimonControl::SimonControl (QWidget* parent) : QWidget (parent)
{
  setFocusPolicy(Qt::StrongFocus);  // per rebre events de teclat
  connect (&timer, SIGNAL (timeout()), this, SLOT (enciendeLuz()));
}

void SimonControl::empiezaJuego ()
{
  emit bloquearInput ();
  emit startGame ();
  timer.stop();
  indice_jugador = 0;
  puntuacion = 0;

  emit mostrarPuntuacion(puntuacion);

  ronda = 0;
  generarSecuencia ();
  iniciarTimer ();
}

void SimonControl::iniciarTimer ()
{
  emit mostrarMensaje (QString::fromStdString ("¡Atento!"));
  timer.start (1000);
  indice_jugador = 0;
  if (ronda < 20) ++ronda;
  emit mostrarRonda(ronda);
  contador_ronda = 0;
  emit bloquearInput ();
}

void SimonControl::generarSecuencia() 
{
  srand (time(NULL));
  for (int i = 0; i < 20; ++i)
  {
    switch (rand() % 4) {
      case 0:
        secuencia[i] = ROJO;
      break;
      case 1:
        secuencia[i] = VERDE;
      break;
      case 2:
        secuencia[i] = AZUL;
      break;
      default:
        secuencia[i] = AMARILLO;
      break;
    }
  }
}

//Se ejecuta cada 1000 ms.
void SimonControl::enciendeLuz () 
{
  if (contador_ronda < ronda) 
  {
    if (not enciende_gris) {
      switch (secuencia[contador_ronda]) {
        case ROJO:
          emit enviarColor (0xff0000);
        break;
        case VERDE:
          emit enviarColor (0x00ff00);
        break;
        case AZUL:
          emit enviarColor (0x0000ff);
        break;
        default:
          emit enviarColor (0xffff00);
        break;
      }
      enciende_gris = true;
      ++contador_ronda;
    }
    else {
      emit enviarColor (0x000000);     //Enviar gris
      enciende_gris = false;
    }
  }
  else 
  {
    timer.stop();
    mensaje();
    emit enviarColor (0x000000);
    enciende_gris = false;
    emit leerInput ();
  }
  update();
}

void SimonControl::recibeRojo() 
{
  if (ROJO == secuencia[indice_jugador]) {
    ++indice_jugador;
    ++puntuacion;
    if (indice_jugador == ronda) iniciarTimer();
    emit mostrarPuntuacion (puntuacion);
    }
  else jugadorFalla();
}

void SimonControl::recibeVerde() 
{
  if (VERDE == secuencia[indice_jugador]) {
    ++indice_jugador;
    ++puntuacion;
    if (indice_jugador == ronda) iniciarTimer();
    emit mostrarPuntuacion (puntuacion);
  }
  else jugadorFalla();
}

void SimonControl::recibeAzul() 
{
  if (AZUL == secuencia[indice_jugador]) {
    ++indice_jugador;
    ++puntuacion;
    if (indice_jugador == ronda) iniciarTimer();
    emit mostrarPuntuacion (puntuacion);
  }
  else jugadorFalla();
}

void SimonControl::recibeAmarillo()
{
  if (AMARILLO == secuencia[indice_jugador]) {
    ++indice_jugador;
    ++puntuacion;
    if (indice_jugador == ronda) iniciarTimer();
    emit mostrarPuntuacion (puntuacion);
  }
  else jugadorFalla();
}

void SimonControl::jugadorFalla ()
{
  timer.stop();
  indice_jugador = 0;
  ronda = 0;
  contador_ronda = 0;

  int a = rand()%2;
  if (a == 0) emit mostrarMensaje (QString::fromStdString ("¡Has perdido!"));
  else emit mostrarMensaje (QString::fromStdString ("¡Has perdido! Yo también me agobiaría con tantos colores."));

  emit bloquearInput ();

  emit derrota (); 
}

void SimonControl::juegoReiniciado ()
{
  jugadorFalla();
  puntuacion = 0;
  enciende_gris = false;

  emit enviarColor (0x000000);     //Enviar gris
  emit mostrarMensaje (QString::fromStdString ("¡Vuelta a empezar!"));
  emit mostrarPuntuacion(puntuacion);
  emit mostrarRonda(ronda);
}

void SimonControl::mensaje ()
{
  if (ronda == 1) emit mostrarMensaje (QString::fromStdString ("Es tu turno, ¡no te equivoques!"));
  else if (ronda == 2) emit mostrarMensaje (QString::fromStdString ("La segunda ronda son dos colores, ¿los recuerdas bien?"));
  else if (ronda == 5) emit mostrarMensaje (QString::fromStdString ("¿Todavía sigues en pie? ¡Eres un crack!"));
  else if (ronda == 10) emit mostrarMensaje (QString::fromStdString ("Vas por la mitad, la emoción aumenta entre el público."));
  else if (ronda == 15) emit mostrarMensaje (QString::fromStdString ("Yo ya me he perdido con tantos colores, ¿y tú?"));
  else if (ronda == 20) emit mostrarMensaje (QString::fromStdString ("¡Última ronda! Si has llegado hasta aquí, esto te será pan comido."));
  else {
    switch (rand() % 3)
    {
      case 0:
        emit mostrarMensaje (QString::fromStdString 
        ("Es tu turno, un solo fallo supone la derrota inmediata."));        
      break;
      case 1:
        emit mostrarMensaje (QString::fromStdString 
        ("¡Es tu turno!"));        
      break;
      default:
        emit mostrarMensaje (QString::fromStdString 
        ("Es tu turno, ¿lo conseguirás?"));        
      break;
    }
  }
}


