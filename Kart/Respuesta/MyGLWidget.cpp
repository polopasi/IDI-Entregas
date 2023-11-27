#include "MyGLWidget.h"
#include <iostream>
#include <stdio.h>
#include <QTimer>

#define printOpenGLError() printOglError(__FILE__, __LINE__)
#define CHECK() printOglError(__FILE__, __LINE__,__FUNCTION__)
#define DEBUG() std::cout << __FILE__ << " " << __LINE__ << " " << __FUNCTION__ << std::endl;

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

MyGLWidget::~MyGLWidget()
{
}

void MyGLWidget::initializeGL ()
{
  // Cal inicialitzar l'ús de les funcions d'OpenGL
  initializeOpenGLFunctions();  
  glEnable(GL_DEPTH_TEST);
  
  connect (&timer, SIGNAL (timeout()), this, SLOT (animar ()));
  glClearColor(0.5, 0.7, 1.0, 1.0); // defineix color de fons (d'esborrat)
  carregaShaders();
  creaBuffersModels();
  creaBuffersTerra();
  iniEscena();
  iniCamera();
}

void MyGLWidget::keyPressEvent(QKeyEvent* event) 
{
  makeCurrent();
  switch (event->key()) {
    case Qt::Key_Up: { 
      posicionCoche[0] += 3*M_PI/180;
      posicionCoche[1] += 2*M_PI/180;
      posicionCoche[2] += 1*M_PI/180;
      break;
    }
    case Qt::Key_C: { 
        conduciendo = !conduciendo;
        theta = (45*M_PI/180);
        psi = 0;
        FOV = 60*M_PI/180;
        viewTransform();
        projectTransform();
        paintGL();
        break;
          }
    case Qt::Key_R: { 
      posicionCoche[0] = posicionCoche[1] = posicionCoche[2] = 0;
      theta = (45*M_PI/180);
      psi = 0;
      conduciendo = false;
      automatico = false;
      timer.stop ();
      viewTransform();
      projectTransform();
      break;
        }  
    case Qt::Key_T: { 
        automatico = !automatico;
        if (automatico) timer.start (1);
        else timer.stop ();
        break;
        }          
    default: event->ignore(); break;
  }
  update();
}

void MyGLWidget::iniCamera(){
  raw = float (width()/height());
  theta = (45*M_PI/180);
  psi = 0;
  puntoMinimo = glm::vec3 (-10.0f, 0., -10.0f);
  puntoMaximo = glm::vec3 (10.0f, 0., 10.0f);
  jugador = glm::vec3 (0, 0.5, 0);
  calcularEscena();
  viewTransform();
  projectTransform();
}

void MyGLWidget::iniEscena()
{
}


void MyGLWidget::resizeGL (int w, int h) 
{
// Aquest codi és necessari únicament per a MACs amb pantalla retina.
#ifdef __APPLE__
  GLint vp[4];
  glGetIntegerv (GL_VIEWPORT, vp);
  ample = vp[2];
  alt = vp[3];
#else
  ample = w;
  alt = h;
#endif
    rav = float(ample)/float(alt);  
    raw = rav;
    if (rav < 1) FOV = 2.0*atan (tan (FOVoptimo/2.0)/rav);
  projectTransform();
}

void MyGLWidget::projectTransform ()
{
  glm::mat4 Proj(1.0f);
  float zn;
  if (not conduciendo) zn = distanciaEscena - radioEscena;
  else zn = 0.25;
  float zf = distanciaEscena + radioEscena;
  Proj = glm::perspective (FOV, raw, zn, zf);
  glUniformMatrix4fv (projLoc, 1, GL_FALSE, &Proj[0][0]);
}

void MyGLWidget::viewTransform() {
  glm::mat4 View (1.0);

  if (not conduciendo) {
    View = glm::translate (View, glm::vec3(0.,0.,-distanciaEscena));
    View = glm::rotate (View, theta, glm::vec3 (1., 0., 0.));
    View = glm::rotate (View, psi, glm::vec3 (0., 1., 0.));
    View = glm::translate (View, -vrpEscena);
  }
  else {
    View = glm::translate (View, glm::vec3(-radioJugador, -0.5, -0.5));
    View = glm::rotate (View, -anguloJugador, glm::vec3(0, 1, 0));
    View = glm::translate (View, -vrpEscena);
  }
  glUniformMatrix4fv (viewLoc, 1, GL_FALSE, &View[0][0]);
}

void MyGLWidget::creaBuffersTerra ()
{
  //LL2GLWidget::creaBuffersTerra();
  
  // VBO amb la posició dels vèrtexs
  glm::vec3 posTerra[4] = {
        glm::vec3(-10.0, 0.0, -10.0),
        glm::vec3(-10.0, 0.0,  10.0),
        glm::vec3( 10.0, 0.0, -10.0),
        glm::vec3( 10.0, 0.0,  10.0)
  }; 

  glm::vec3 c(0.65, 0.2, 0.05);
  glm::vec3 colTerra[4] = { c, c, c, c };

  // VAO
  glGenVertexArrays(1, &VAO_Terra);
  glBindVertexArray(VAO_Terra);

  GLuint VBO_Terra[2];
  glGenBuffers(2, VBO_Terra);

  // geometria
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Terra[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(posTerra), posTerra, GL_STATIC_DRAW);
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  // color
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Terra[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(colTerra), colTerra, GL_STATIC_DRAW);
  glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(colorLoc);
  
  glBindVertexArray (0);
}

void MyGLWidget::paintGL ()
{
  // descomentar per canviar paràmetres
  // glViewport (0, 0, ample, alt);

  // Esborrem el frame-buffer
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Road
  glBindVertexArray (VAO_models[ROAD]);
  for (int i = 0; i < 4; ++i) {
    float angleAux = float(i*90.0f*M_PI/180);
    glm::vec3 positionRoad;
    switch (i) {
      case 0:
        positionRoad = glm::vec3(5, 0, -5);
        break;
      case 1:
        positionRoad = glm::vec3(-5, 0, -5);
        break;
      case 2:
        positionRoad = glm::vec3(-5, 0, 5);
        break;
      default:
        positionRoad = glm::vec3(5, 0, 5);
        break;
    }
    RoadTransform(positionRoad, angleAux);
    glDrawArrays(GL_TRIANGLES, 0, models[ROAD].faces().size()*3);
  }

  // Car
  glBindVertexArray (VAO_models[CAR]);
  for (int i = 0; i < 3; ++i) {
    float radioInicial = 6 + 1.5*i;
    CarTransform(radioInicial, posicionCoche[i]);
    glm::vec3 color = glm::vec3(1, 1, 1);     
    switch (i) {
      case 0:
        color = glm::vec3(0, 0, 1);     
        break;
      case 1:
        color = glm::vec3(0, 1, 0);
        //Jugador
        radioJugador = radioInicial;
        anguloJugador = posicionCoche[i];
        if (conduciendo) viewTransform(); 
        break;
      default:
        color = glm::vec3(1, 0, 0);     
        break;
    }
    glUniform3fv(colorCocheLoc, 1, &color[0]);
    glDrawArrays(GL_TRIANGLES, 0, models[CAR].faces().size()*3);
    color = glm::vec3(1, 1, 1);     
    glUniform3fv(colorCocheLoc, 1, &color[0]);
  }


  // Pipe
  glBindVertexArray (VAO_models[PIPE]);
  PipeTransform();
  glDrawArrays(GL_TRIANGLES, 0, models[PIPE].faces().size()*3);
  
  // Terra
  glBindVertexArray (VAO_Terra);
  TerraTransform();
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  glBindVertexArray (0);

}

void MyGLWidget::RoadTransform (glm::vec3 pos, float angle)
{
  glm::mat4 TG(1.0f);  
  TG = glm::translate(TG, pos);
  TG = glm::translate(TG, glm::vec3 (0, 0.01, 0));
  TG = glm::scale(TG, glm::vec3(5*escalaModels[ROAD]));
  TG = glm::rotate(TG, angle, glm::vec3 (0, 1, 0));
  TG = glm::translate(TG, -centreBaseModels[ROAD]);
  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::PipeTransform ()
{
  glm::mat4 TG(1.0f);
  /*
    Como se ha escalado a 1, (profunditatDesitjada/max - min), solo es necesario
    multiplicar por 3 para que la tuberia sea de tamanno 3.
  */
  TG = glm::scale(TG, glm::vec3(3*escalaModels[PIPE]));
  TG = glm::translate(TG, -centreBaseModels[PIPE]);
  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &TG[0][0]);
}


void MyGLWidget::CarTransform (float radi, float angle)
{
  glm::mat4 TG(1.0f);
  TG = glm::rotate(TG, angle, glm::vec3(0,1,0));
  TG = glm::translate(TG, glm::vec3(radi,0,0));
  TG = glm::scale(TG, glm::vec3(escalaModels[CAR]));
  TG = glm::translate(TG, -centreBaseModels[CAR]);
  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}


void MyGLWidget::carregaShaders()
{
  // Creem els shaders per al fragment shader i el vertex shader
  QOpenGLShader fs (QOpenGLShader::Fragment, this);
  QOpenGLShader vs (QOpenGLShader::Vertex, this);
  // Carreguem el codi dels fitxers i els compilem
  fs.compileSourceFile("shaders/basicShader.frag");
  vs.compileSourceFile("shaders/basicShader.vert");
  // Creem el program
  program = new QOpenGLShaderProgram(this);
  // Li afegim els shaders corresponents
  program->addShader(&fs);
  program->addShader(&vs);
  // Linkem el program
  program->link();
  // Indiquem que aquest és el program que volem usar
  program->bind();

  // Identificador per als  atributs
  vertexLoc = glGetAttribLocation (program->programId(), "vertex");
  colorLoc = glGetAttribLocation (program->programId(), "color");

  // Identificadors dels uniform locations
  transLoc = glGetUniformLocation(program->programId(), "TG");
  projLoc  = glGetUniformLocation (program->programId(), "Proj");
  viewLoc  = glGetUniformLocation (program->programId(), "View");
  colorCocheLoc  = glGetUniformLocation (program->programId(), "colorCoche");
}

void MyGLWidget::calculaCapsaModel (Model &p, float &escala, float profunditatDesitjada, glm::vec3 &centreBase)
{
  // Càlcul capsa contenidora i valors transformacions inicials
  float minx, miny, minz, maxx, maxy, maxz;
  minx = maxx = p.vertices()[0];
  miny = maxy = p.vertices()[1];
  minz = maxz = p.vertices()[2];
  for (unsigned int i = 3; i < p.vertices().size(); i+=3)
  {
    if (p.vertices()[i+0] < minx)
      minx = p.vertices()[i+0];
    if (p.vertices()[i+0] > maxx)
      maxx = p.vertices()[i+0];
    if (p.vertices()[i+1] < miny)
      miny = p.vertices()[i+1];
    if (p.vertices()[i+1] > maxy)
      maxy = p.vertices()[i+1];
    if (p.vertices()[i+2] < minz)
      minz = p.vertices()[i+2];
    if (p.vertices()[i+2] > maxz)
      maxz = p.vertices()[i+2];
  }

  if (miny < puntoMinimo.y) 
    puntoMinimo = glm::vec3 (puntoMinimo.x, miny, puntoMinimo.z);
  if (maxy > puntoMaximo.y) 
    puntoMaximo = glm::vec3 (puntoMaximo.x, maxy, puntoMaximo.z);

  escala = profunditatDesitjada/(maxz-minz);
  centreBase[0] = (minx+maxx)/2.0; centreBase[1] = miny; centreBase[2] = (minz+maxz)/2.0;
}

void MyGLWidget::mouseMoveEvent(QMouseEvent *e) {
	if (not conduciendo) 
  {
    makeCurrent();

    psi += float(2*M_PI*(e->x() - mouseX_ant)/width());
    theta += float(2*M_PI*(e->y() - mouseY_ant)/height());


    viewTransform();
    update();
    mouseX_ant = e->x();
    mouseY_ant = e->y();
  }
}


void MyGLWidget::mousePressEvent(QMouseEvent *e) {
  mouseX_ant = e->x();
  mouseY_ant = e->y();
}

void MyGLWidget::calcularEscena () {
  float xMedio = (puntoMaximo.x + puntoMinimo.x)/2;
  float yMedio = (puntoMaximo.y + puntoMinimo.y)/2;
  float zMedio = (puntoMaximo.z + puntoMinimo.z)/2;
  vrpEscena = glm::vec3 (xMedio, yMedio, zMedio);

  radioEscena = glm::length (puntoMaximo - puntoMinimo)/2;
  distanciaEscena = 2*radioEscena;

  FOVoptimo = FOV = 2 * glm::asin(radioEscena/distanciaEscena);

}

void MyGLWidget::animar () {
  makeCurrent ();
  posicionCoche[0] += 3*M_PI/180;
  posicionCoche[1] += 2*M_PI/180;
  posicionCoche[2] += 1*M_PI/180;
  update ();
}