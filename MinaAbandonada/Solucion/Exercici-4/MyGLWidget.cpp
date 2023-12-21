// MyGLWidget.cpp
#include "MyGLWidget.h"
#include <iostream>
#include <stdio.h>

#define printOpenGLError() printOglError(__FILE__, __LINE__)
#define CHECK() printOglError(__FILE__, __LINE__,__FUNCTION__)
#define DEBUG() std::cout << __FILE__ << " " << __LINE__ << " " << __FUNCTION__ << std::endl;

MyGLWidget::MyGLWidget(QWidget *parent=0) : LL4GLWidget(parent) {
  psi = 0;
  theta = 30*M_PI/180;
  connect (&timer, SIGNAL (timeout()), this, SLOT (animar ()));  
}

int MyGLWidget::printOglError(const char file[], int line, const char func[]) 
{
    GLenum glErr;
    int    retCode = 0;

    glErr = glGetError();
    const char * error = 0;
    switch (glErr)
    {
        case 0x0500:
            error = "GL_INVALID_ENUM";
            break;
        case 0x501:
            error = "GL_INVALID_VALUE";
            break;
        case 0x502: 
            error = "GL_INVALID_OPERATION";
            break;
        case 0x503:
            error = "GL_STACK_OVERFLOW";
            break;
        case 0x504:
            error = "GL_STACK_UNDERFLOW";
            break;
        case 0x505:
            error = "GL_OUT_OF_MEMORY";
            break;
        default:
            error = "unknown error!";
    }
    if (glErr != GL_NO_ERROR)
    {
        printf("glError in file %s @ line %d: %s function: %s\n",
                             file, line, error, func);
        retCode = 1;
    }
    return retCode;
}

MyGLWidget::~MyGLWidget() {
}

void MyGLWidget::mouseMoveEvent(QMouseEvent *e)
{
  makeCurrent();
  // Aqui cal que es calculi i s'apliqui la rotacio o el zoom com s'escaigui...
  if (DoingInteractive == ROTATE)
  {

    psi += float(2*M_PI*(e->x() - xClick)/width());
    theta += float(2*M_PI*(e->y() - yClick)/height());


  viewTransform();
  }
  xClick = e->x();
  yClick = e->y();
  iluminacion();
  iluminacionLinternas();
  update ();
}

void MyGLWidget::keyPressEvent(QKeyEvent* event) {
  makeCurrent();
  switch (event->key()) {
  case Qt::Key_A: {
      angulo_minecart += 5;
      angulo_minecart = float(int(angulo_minecart)%360);
      paintGL();
    break;
	}
  case Qt::Key_L: {
      lucesFuera();
    break;
	}
  case Qt::Key_R: {
    break;
	}
  case Qt::Key_S: {
      automatico = !automatico;
      if (automatico) timer.start (50);
      else timer.stop();
    break;
	}	
  default: LL4GLWidget::keyPressEvent(event); break;
  }
  iluminacion();
  paintGL();
  update();
}

void MyGLWidget::modelTransformTerra ()
{
  glm::mat4 TG = glm::mat4(1.f);  // Matriu de transformació
  TG = glm::scale (TG, glm::vec3 (3, 1, 3));
  TG = glm::translate (TG, -glm::vec3(5,0,5));
  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::viewTransform () 
{
  
  glm::mat4 View (1.0);
  View = glm::translate (View, glm::vec3(0.,0.,-42));
  View = glm::rotate (View, theta, glm::vec3 (1., 0., 0.));
  View = glm::rotate (View, psi, glm::vec3 (0., 1., 0.));
  View = glm::translate (View, centreEsc);
  glUniformMatrix4fv (viewLoc, 1, GL_FALSE, &View[0][0]);
}

void MyGLWidget::iniEscena ()
{
  posFocus_Loc = glGetUniformLocation (program->programId(), "posFocus");
  colorFocus_Loc = glGetUniformLocation (program->programId(), "colorFocus");
  posicionLinterna1_Loc = glGetUniformLocation (program->programId(), "posLinterna1Vertex");
  posicionLinterna2_Loc = glGetUniformLocation (program->programId(), "posLinterna2Vertex");
  colorLinterna_Loc = glGetUniformLocation (program->programId(), "colorLinterna");
  TGfocoLoc = glGetUniformLocation (program->programId(), "TGfoco");

  iluminacion();
  iluminacionLinternas();
  centreEsc = glm::vec3(0,0,0);
  radiEsc = 21;  

}


void MyGLWidget::iniMaterialTerra ()
{
  // Donem valors al material del terra
  amb = glm::vec3(0.1,0.0,0.0);
  diff = glm::vec3(0.4,0.0,0.0);
  spec = glm::vec3(0.8,0.8,0.8);
  shin = 100;
}

void MyGLWidget::iluminacion ()
{

  posFocus = glm::vec3 (1., 1., 1.);
  glUniform3fv(posFocus_Loc, 1, &posFocus[0]);
  glUniform3fv(colorFocus_Loc, 1, &colorFocus[0]);
}

void MyGLWidget::iluminacionLinternas ()
{
  posLinterna1Vertex = glm::vec3 (0.5, 0.5,-2.0);
  posLinterna2Vertex = glm::vec3 (-0.5, 0.5,-2.0);
  glUniform3fv(posicionLinterna1_Loc, 1, &posLinterna1Vertex[0]);
  glUniform3fv(posicionLinterna2_Loc, 1, &posLinterna2Vertex[0]);

  colorLinterna = glm::vec3 (0.6, 0.6, 0.0);
  glUniform3fv(colorLinterna_Loc, 1, &colorLinterna[0]);
}

void MyGLWidget::paintGL () 
{
  // En cas de voler canviar els paràmetres del viewport, descomenteu la crida següent i
  // useu els paràmetres que considereu (els que hi ha són els de per defecte)
  // glViewport (0, 0, ample, alt);
  
  // Esborrem el frame-buffer i el depth-buffer
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //FOCOS
  iluminacion();
  iluminacionLinternas();
  modelTransformFoco();


  // TERRA
  glBindVertexArray (VAO_Terra);
  modelTransformTerra ();
  glDrawArrays(GL_TRIANGLES, 0, 6);
  
  // TRACK
  glBindVertexArray (VAO_Track);
  for (int i = 0; i < 90; ++i) {
    modelTransformTrack (10,4*i);
    glDrawArrays(GL_TRIANGLES, 0, track.faces().size()*3);
  }
  
  // FANTASMA
  if (!foco_camara) {
    glBindVertexArray (VAO_Fantasma);
    modelTransformFantasma ();
    glDrawArrays(GL_TRIANGLES, 0, fantasma.faces().size()*3);
  }

  // MINE CART
  glBindVertexArray (VAO_MineCart);
  modelTransformMineCart ();
  glDrawArrays(GL_TRIANGLES, 0, mineCart.faces().size()*3);
  
  glBindVertexArray(0);
}

void MyGLWidget::modelTransformTrack (float radi, float rotY)
{
  glm::mat4 TG = glm::mat4 (1.f);
  TG = glm::rotate(TG, float(glm::radians(rotY)), glm::vec3(0,1,0));
  TG = glm::translate(TG, glm::vec3(radi,0,0));
  TG = glm::scale(TG, glm::vec3(escalaTrack, escalaTrack, escalaTrack));
  TG = glm::rotate(TG, float(M_PI/2), glm::vec3(0,1,0));
  TG = glm::translate(TG, -centreBaseTrack);
  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::modelTransformMineCart ()
{
  glm::mat4 TG = glm::mat4(1.f);
  TG = glm::rotate(TG, float(glm::radians(angulo_minecart)), glm::vec3(0,1,0));
  TG = glm::translate(TG, glm::vec3(10,0,0));
  TG = glm::scale(TG, glm::vec3(escalaMineCart, escalaMineCart, escalaMineCart));
  TG = glm::translate(TG, -centreBaseMineCart);
  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::modelTransformFantasma () 
{
  glm::mat4 TG  = glm::mat4(1.f);
  TG = glm::rotate(TG, float(glm::radians(angulo_minecart)), glm::vec3(0,1,0));
  TG = glm::translate(TG, glm::vec3(10,0,0));
  TG = glm::scale(TG, glm::vec3(0.75, 0.75, 0.75));
  TG = glm::translate(TG, glm::vec3(0, 1.0, 0));
  TG = glm::rotate(TG, float(glm::radians(180.0f)), glm::vec3(0,1,0));
  TG = glm::scale(TG, glm::vec3(escalaFantasma, escalaFantasma, escalaFantasma));
  TG = glm::translate(TG, -centreBaseFantasma);
  
  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::modelTransformFoco ()
{
  glm::mat4 TG = glm::mat4(1.f);
  TG = glm::rotate(TG, float(glm::radians(angulo_minecart)), glm::vec3(0,1,0));
  TG = glm::translate(TG, glm::vec3(10,0,0));
  TG = glm::scale(TG, glm::vec3(escalaMineCart, escalaMineCart, escalaMineCart));
  TG = glm::translate(TG, -centreBaseMineCart);
  glUniformMatrix4fv (TGfocoLoc, 1, GL_FALSE, &TG[0][0]);

  TG = glm::mat4(1.f);
  TG = glm::rotate(TG, float(glm::radians(angulo_minecart)), glm::vec3(0,1,0));
  TG = glm::translate(TG, glm::vec3(10,0,0));
  TG = glm::scale(TG, glm::vec3(escalaMineCart, escalaMineCart, escalaMineCart));
  TG = glm::translate(TG, -centreBaseMineCart);
  glUniformMatrix4fv (TGfocoLoc, 1, GL_FALSE, &TG[0][0]);

}

void MyGLWidget::lucesFuera ()
{
  foco_camara = !foco_camara;
  if (!foco_camara) {
    colorFocus = glm::vec3 (0.0, 0.0, 0.0);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
  }
  else {
    colorFocus = glm::vec3 (0.8, 0.8, 0.8);
    glClearColor(0.5, 0.7, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
  }
}


void MyGLWidget::animar () {
  makeCurrent ();
  angulo_minecart += 5;
  angulo_minecart = float(int(angulo_minecart)%360);
  update ();
}