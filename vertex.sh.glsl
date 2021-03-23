#version 130

in vec3 i_direction;
in vec2 i_pos;

uniform mat4 transfrom;

out vec3 o_direction;

void main() {

    o_direction = (transfrom*vec4(i_direction,1)).xyz;
    gl_Position = i_pos;

}