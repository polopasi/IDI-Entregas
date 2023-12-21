// MyGLWidget.h
#include "LL4GLWidget.h"
#include <QTimer>

class MyGLWidget : public LL4GLWidget {
  Q_OBJECT
  public:
    MyGLWidget(QWidget *parent);
    ~MyGLWidget();
  protected:
    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void keyPressEvent(QKeyEvent* event);
    virtual void iniEscena ();
    virtual void iniMaterialTerra ();
    virtual void viewTransform ();
    virtual void paintGL();
    virtual void modelTransformTerra ();
    virtual void modelTransformTrack(float radi, float rotY);
    virtual void modelTransformMineCart ();
    virtual void modelTransformFantasma ();
    virtual void iluminacion ();
    virtual void iluminacionLinternas();
    virtual void lucesFuera ();
    virtual void modelTransformFoco();

public slots:
    void animar ();

  private:
    int printOglError(const char file[], int line, const char func[]);
    float psi, theta;
    bool foco_camara = true;
    float angulo_minecart = 0;
    
    //Foco de observador
    glm::vec3 posFocus;
    glm::vec3 colorFocus = glm::vec3 (0.8, 0.8, 0.8);

    //Foco de linternas
    glm::vec3 posLinterna1Vertex;
    glm::vec3 posLinterna2Vertex;
    glm::vec3 colorLinterna;

    //Atributes pointers:
    GLuint posFocus_Loc;
    GLuint colorFocus_Loc;
    GLuint posicionLinterna1_Loc;
    GLuint posicionLinterna2_Loc;
    GLuint colorLinterna_Loc;
    GLuint TGfocoLoc;

    //Animacion
    QTimer timer;
    bool automatico = false;
};
