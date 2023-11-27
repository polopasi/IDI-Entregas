#define GLM_FORCE_RADIANS
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLWidget>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QKeyEvent>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

using std::vector;

class MyGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
  Q_OBJECT

  public:
    MyGLWidget (QWidget *parent=0);
    ~MyGLWidget ();
    
  protected:
    // initializeGL - Aqui incluim les inicialitzacions del contexte grafic.
    virtual void initializeGL ();

    // paintGL - Mètode cridat cada cop que cal refrescar la finestra.
    // Tot el que es dibuixa es dibuixa aqui.
    virtual void paintGL ();
 
    // resize - Es cridat quan canvia la mida del widget
    virtual void resizeGL (int width, int height);  

    // keyPressEvent - Es cridat quan es prem una tecla
    virtual void keyPressEvent (QKeyEvent *event);

  private:
      
    void pintaMolinet (glm::vec3 posicioBase, float escala);  
      
    void creaBuffersPal();
    void creaBuffersAspa();
    
    void carregaShaders();
    
    void modelTransformPal(glm::vec3 posicioBase, float escala);
    void modelTransformAspa(glm::vec3 posicioPivot, float angle, float escala);
    
    // program
    QOpenGLShaderProgram *program;

    //atributos de clase
    float angulo = 0;   //La posición de las aspas es i*60*M_PI/180, 
                        //donde i es el número del aspa.
    bool multimolinet = false;

    //la variable color se utiliza de forma "casual" para colores sueltos, como el del palo.
    glm::vec3 color;

    //Colores de las aspas en un vector, en el orden en el que se pintan las aspas, 
    //en sentido antihorario.
    vector <glm::vec3> coloresAspas = {

      glm::vec3 (1, 0, 0),    //Rojo       
      glm::vec3 (0, 1, 0),    //Verde
      glm::vec3 (0, 0, 1),    //Azul
      glm::vec3 (1, 1, 0),    //Amarillo
      glm::vec3 (0, 1, 1),    //Cian
      glm::vec3 (1, 0, 1),    //Magenta
    };
    
    // attribute locations
    GLuint vertexLoc;
    GLuint colorLoc;
    
    // uniform locations
    GLuint TGLoc;
    GLuint colorinLoc;

    // VAOs
    GLuint VAOPal;
    GLuint VAOAspa;

    // viewport
    GLint ample, alt;    
};
