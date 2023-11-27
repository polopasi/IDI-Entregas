#include "MyGLWidget.h"

#include <iostream>

MyGLWidget::MyGLWidget (QWidget* parent) : QOpenGLWidget(parent), program(NULL)
{
  setFocusPolicy(Qt::StrongFocus);  // per rebre events de teclat
}

MyGLWidget::~MyGLWidget ()
{
  if (program != NULL)
    delete program;
}

void MyGLWidget::initializeGL ()
{
  // Cal inicialitzar l'ús de les funcions d'OpenGL
  initializeOpenGLFunctions();
  
  glClearColor (0.5, 0.7, 1.0, 1.0); // defineix color de fons (d'esborrat)
  carregaShaders();
  creaBuffersPal();
  creaBuffersAspa();
}


void MyGLWidget::modelTransformPal(glm::vec3 posicioBase, float escala)
{
  glm::mat4 TG(1.0f);
  TG = glm::translate (TG, posicioBase);
  TG = glm::scale (TG, glm::vec3 (escala, escala, 0.0));
  glUniformMatrix4fv(TGLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::modelTransformAspa(glm::vec3 posicioPivot, float angle, float escala)
{
  glm::mat4 TG(1.0f);
  TG = glm::translate (TG, posicioPivot);
  TG = glm::scale (TG, glm::vec3 (escala, escala, 0.0));
  TG = glm::rotate(TG, angle, glm::vec3(0.,0.,1.0f));
  glUniformMatrix4fv(TGLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::paintGL ()
{
// En cas de voler canviar els paràmetres del viewport, descomenteu la crida següent i
// useu els paràmetres que considereu (els que hi ha són els de per defecte)
//  glViewport (0, 0, ample, alt);
  
  glClear (GL_COLOR_BUFFER_BIT);  // Esborrem el frame-buffer

  if (multimolinet) {

    pintaMolinet(glm::vec3( 0.0,  0.0,  0.0), 0.25f);  
    pintaMolinet(glm::vec3(-0.5, 0.25, 0.0), 0.35f);  
    pintaMolinet(glm::vec3( 0.5, -0.5, 0.0), 0.45f);  
  }
  else {

    pintaMolinet(glm::vec3(0,0,0), 1.0f);  
  }

}

void MyGLWidget::pintaMolinet(glm::vec3 posicioBase, float escala)
{
  // Pintem Pal
  glBindVertexArray(VAOPal);  
  modelTransformPal(posicioBase, escala);
  /*
   * Se pasa el colorin (uniform) blanco, porque en el vertexShader se multiplica 
   * color (variable in) por colorin (uniform) de esta forma al hacer el producto 
   * se mantendrá (color blanco son todo 1.0) el color por defecto del palo.
  */
  color = glm::vec3(1, 1, 1);     
  glUniform3fv(colorinLoc, 1, &color[0]);
  glDrawArrays(GL_TRIANGLES, 0, 6); 

  // Pintamos Aspes
  for (int i = 0; i < 6; ++i)  {
    glBindVertexArray(VAOAspa);  
    
    modelTransformAspa(posicioBase, i*60*M_PI/180 + angulo, escala);
    color = coloresAspas[i];
    glUniform3fv(colorinLoc, 1, &color[0]);
    glDrawArrays(GL_TRIANGLES, 0, 6); 
  }
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
}

void MyGLWidget::keyPressEvent(QKeyEvent* event)
{
  makeCurrent();
  switch (event->key()) {
    case Qt::Key_A: 
      angulo -= 2*M_PI/180;
    	break;
    case Qt::Key_D: 
      angulo += 2*M_PI/180;
    	break;		
    case Qt::Key_3: 
      multimolinet = true;
    	break;		
    case Qt::Key_1: 
      multimolinet = false;
    	break;		
    default: event->ignore(); break;
  }
  update();
}

void MyGLWidget::creaBuffersPal ()
{
  glm::vec3 Vertices[6];  // vèrtexs amb X, Y i Z
  
  // minX = -0.05
  // maxX = 0.05
  // minY = -1.0
  // maxY = 0.0
  
  Vertices[0] = glm::vec3(-0.05, -1.0, 0);
  Vertices[1] = glm::vec3( 0.05, -1.0, 0);
  Vertices[2] = glm::vec3( 0.025,  0.0, 0);
  Vertices[3] = glm::vec3( 0.025,  0.0, 0);
  Vertices[4] = glm::vec3(-0.025,  0.0, 0);
  Vertices[5] = glm::vec3(-0.05, -1.0, 0);

  glm::vec3 Colors[6];  // colors amb R, G i B
  
  Colors[0] = glm::vec3(0.5, 0.5, 0.5);
  Colors[1] = glm::vec3(0.5, 0.5, 0.5);
  Colors[2] = glm::vec3(0.5, 0.5, 0.5);
  Colors[3] = glm::vec3(0.5, 0.5, 0.5);
  Colors[4] = glm::vec3(0.5, 0.5, 0.5);
  Colors[5] = glm::vec3(0.5, 0.5, 0.5);

  
  // Creació del Vertex Array Object (VAO) que usarem per pintar el quadrat
  glGenVertexArrays(1, &VAOPal);
  glBindVertexArray(VAOPal);

  // Creació dels Vertex Buffer Objects (VBOs) amb les posicions i els colors dels vèrtexs
  GLuint VBOs[2];
  glGenBuffers(2, &VBOs[0]);
  
  // Omplim el VBO de posicions
  glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  // Omplim el VBO de colors
  glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Colors), Colors, GL_STATIC_DRAW);
  glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(colorLoc);


  // Desactivem el VAO
  glBindVertexArray(0);
}

void MyGLWidget::creaBuffersAspa ()
{
  glm::vec3 Vertices[6];  // vèrtexs amb X, Y i Z
  
  // minX = 0.0
  // maxX = 0.65
  // minY = 0.0
  // maxY = 0.3
  
  Vertices[0] = glm::vec3(0.0,  0.0,  0);
  Vertices[1] = glm::vec3(0.65,  0.0,  0);
  Vertices[2] = glm::vec3(0.25, 0.3,  0);
  Vertices[3] = glm::vec3(0.0,  0.0,  0);
  Vertices[4] = glm::vec3(0.25, 0.3,  0);
  Vertices[5] = glm::vec3(0.0,  0.25, 0);

  glm::vec3 Colors[6];  // colors amb R, G i B
  
  Colors[0] = glm::vec3(1.0, 1.0, 1.0);
  Colors[1] = glm::vec3(1.0, 1.0, 1.0);
  Colors[2] = glm::vec3(1.0, 1.0, 1.0);
  Colors[3] = glm::vec3(0.75, 0.75, 0.75);
  Colors[4] = glm::vec3(0.75, 0.75, 0.75);
  Colors[5] = glm::vec3(0.75, 0.75, 0.75);

  
  // Creació del Vertex Array Object (VAO) que usarem per pintar el quadrat
  glGenVertexArrays(1, &VAOAspa);
  glBindVertexArray(VAOAspa);

  // Creació dels Vertex Buffer Objects (VBOs) amb les posicions i els colors dels vèrtexs
  GLuint VBOs[2];
  glGenBuffers(2, &VBOs[0]);
  
  // Omplim el VBO de posicions
  glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  // Omplim el VBO de colors
  glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Colors), Colors, GL_STATIC_DRAW);
  glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(colorLoc);


  // Desactivem el VAO
  glBindVertexArray(0);
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

  // Obtenim identificador per a l'atribut “vertex” del vertex shader
  vertexLoc = glGetAttribLocation (program->programId(), "vertex");
  colorLoc = glGetAttribLocation (program->programId(), "color");
  
  // Obtenim els identificadors dels uniforms
  TGLoc = glGetUniformLocation(program->programId(), "TG"); 
  colorinLoc = glGetUniformLocation (program->programId(), "colorin");
}
