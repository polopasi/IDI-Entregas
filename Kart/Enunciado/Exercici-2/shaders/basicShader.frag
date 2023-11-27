#version 330 core

in vec3 fcolor;
in vec3 colorCoche;

out vec4 FragColor;

void main() {

	FragColor = vec4(fcolor, 1);
}

