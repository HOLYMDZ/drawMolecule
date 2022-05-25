#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 inColor;

uniform mat4 view;
uniform mat4 projection;

out vec3 AxeColor;

void main(){
    gl_Position = projection * view * vec4(aPos, 1.0);
    AxeColor = inColor;
}
