#version 330 core

in vec3  fmatamb;
in vec3  fmatdiff;
in vec3  fmatspec;
in float fmatshin;
in vec3  fvertex;
in vec3  fnormal;

in vec3 posLinterna1;
in vec3 posLinterna2;

const vec3 llumAmbient = vec3(0.1, 0.1, 0.1);

out vec4 FragColor;

uniform vec3 posFocus;
uniform vec3 colorFocus;
uniform vec3 colorLinterna;

vec3 Ambient() {
  return llumAmbient*fmatamb;
}

vec3 Difus (vec3 normal, vec3 L, vec3 colFocus)
{
  // Fixeu-vos que SOLS es retorna el terme de Lambert!
  // S'assumeix que els vectors que es reben com a paràmetres estan normalitzats
  vec3 colRes = vec3(0);
  if (dot (L, normal) > 0)
    colRes = colFocus * fmatdiff * dot (L, normal);
  return (colRes);
}

vec3 Especular (vec3 normal, vec3 L, vec3 vertSCO, vec3 colFocus)
{
  // Fixeu-vos que SOLS es retorna el terme especular!
  // Assumim que els vectors estan normalitzats
  vec3 colRes = vec3 (0);
  // Si la llum ve de darrera o el material és mate no fem res
  if ((dot(normal,L) < 0) || (fmatshin == 0))
    return colRes;  // no hi ha component especular

  vec3 R = reflect(-L, normal); // equival a: 2.0*dot(normal,L)*normal - L;
  vec3 V = normalize(-vertSCO); // perquè la càmera està a (0,0,0) en SCO

  if (dot(R, V) < 0)
    return colRes;  // no hi ha component especular

  float shine = pow(max(0.0, dot(R, V)), fmatshin);
  return (colRes + fmatspec * colFocus * shine);
}

void main()
{
  vec3 color = Ambient();
	vec3 normal = normalize (fnormal);
  //FOCO OBSERVADOR
  vec3 L = normalize(posFocus- fvertex);
	color += Difus (normal, L , colorFocus) + Especular (normal, L, fvertex, colorFocus);

  //FOCO LINTERNA 1
  L = normalize(posLinterna1- fvertex);
	color += Difus (normal, L , colorLinterna) + Especular (normal, L, fvertex, colorLinterna);

  //FOCO LINTERNA 2
  L = normalize(posLinterna2- fvertex);
	color += Difus (normal, L , colorLinterna) + Especular (normal, L, fvertex, colorLinterna);

  FragColor = vec4(color, 1);
  //FragColor = vec4(fmatdiff, 1);
}
