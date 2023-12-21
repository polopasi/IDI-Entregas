#version 330 core

in vec3 vertex;
in vec3 normal;

in vec3  matamb;
in vec3  matdiff;
in vec3  matspec;
in float matshin;

uniform mat4 TG;
uniform mat4 Proj;
uniform mat4 View;
uniform mat4 TGfoco;

uniform vec3 posLinterna1Vertex;
uniform vec3 posLinterna2Vertex;

out vec3 posLinterna1;
out vec3 posLinterna2;

out vec3  fmatamb;
out vec3  fmatdiff;
out vec3  fmatspec;
out float fmatshin;
out vec3  fvertex;
out vec3  fnormal;


void main()
{	
    posLinterna1 = vec3(View * TGfoco * vec4 (posLinterna1Vertex, 1.0));
    posLinterna2 = vec3(View * TGfoco * vec4 (posLinterna2Vertex, 1.0));

    // Passem les dades al fragment shader
    fmatamb  = matamb;
    fmatdiff = matdiff;
    fmatspec = matspec;
    fmatshin = matshin;

    mat3 normalMatrix = inverse (transpose (mat3 (View * TG)));
    vec3 normalVertex = normalMatrix * normal;
    fnormal = normalize(normalVertex);

    vec4 vertexSCO = View * TG * vec4 (vertex, 1.0);
    fvertex = vec3(vertexSCO);


    gl_Position = Proj * View * TG * vec4 (vertex, 1.0);
}
