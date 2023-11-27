#include "glm/glm.hpp"
#include <QWidget>
#include <vector>
#include <QTimer>
#include <string>

enum color {ROJO, VERDE, AZUL, AMARILLO};

class SimonControl : public QWidget {

    Q_OBJECT

    private:
        bool enciende_gris = false;
        int ronda = 1;
        int contador_ronda = 0;
        int puntuacion = 0;
        std::vector <color> secuencia = std::vector <color> (20);

        //INFORMACION DEL INPUT DEL JUGADOR
        int indice_jugador = 0;
        
        void generarSecuencia();


        QTimer timer;


    public:
        SimonControl (QWidget* parent = 0);
    
    public slots:

        void empiezaJuego ();

        void enciendeLuz ();

        void recibeRojo();

        void recibeVerde();

        void recibeAzul();

        void recibeAmarillo();

        void iniciarTimer();

        void jugadorFalla ();

        void juegoReiniciado ();

        void mensaje ();

    signals:

        void enviarColor (const int &);

        void bloquearInput ();

        void leerInput ();

        void derrota ();

        void startGame ();

        void mostrarPuntuacion(const int &);

        void mostrarRonda(const int &);

        void mostrarMensaje (const QString &);

        void escribirStart ();

};