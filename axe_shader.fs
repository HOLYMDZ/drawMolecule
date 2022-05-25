#version 460 core

out vec4 FragColor;
in vec3 AxeColor;

void main(){
    FragColor = vec4(AxeColor, 1.0f);
}