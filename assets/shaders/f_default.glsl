#version 450 core

out vec4 fragment_color;

in vec2 v_texcoord;

uniform sampler2D u_texture;

void main() {
    fragment_color = texture(u_texture, v_texcoord);
};
