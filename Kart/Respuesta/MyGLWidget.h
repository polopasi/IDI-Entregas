#include "LL2GLWidget.h"

#include "model.h"

class MyGLWidget : public LL2GLWidget {
  Q_OBJECT

  public:
    MyGLWidget(QWidget *parent=0) : LL2GLWidget(parent) {}
    ~MyGLWidget();

  protected:
    virtual void keyPressEvent (QKeyEvent *event);

    virtual void iniCamera();

    virtual void viewTransform();

    virtual void projectTransform ();

    virtual void creaBuffersTerra ();

    virtual void paintGL ();

    virtual void RoadTransform (glm::vec3 pos, float angle);

    virtual void PipeTransform ();

    virtual void CarTransform (float radi, float angle);

    virtual void carregaShaders();   

    virtual void mouseMoveEvent(QMouseEvent *e);

    virtual void mousePressEvent(QMouseEvent *e);

    virtual void iniEscena();

    virtual void calculaCapsaModel (Model &p, float &escala, float profunditatDesitjada, glm::vec3 &CentreBase);

    virtual void calcularEscena();

    virtual void resizeGL(int w, int h);

    virtual void initializeGL();


  private:
  
    int printOglError(const char file[], int line, const char func[]);

    GLuint colorCocheLoc;

    float posicionCoche[3] = {0, 0, 0};

    //Informacion de la escena:
    glm::vec3 puntoMaximo, puntoMinimo, vrpEscena;
    float radioEscena, distanciaEscena, zNear, zFar;
    float FOVoptimo, FOV;

    //Euler
    float theta;
    float psi;
    float mouseX_ant = 0;
    float mouseY_ant = 0;

    //Primera persona
    glm::vec3 jugador;
    bool conduciendo = false;
    float radioJugador = 7.5;
    float anguloJugador;

    //Resize
    float rav, raw;

    //Animacion
    QTimer timer;
    bool automatico = false;

  public slots:
    void animar ();

   
};
