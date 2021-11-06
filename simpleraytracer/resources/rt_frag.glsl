#version 460 core
in vec2 tex_coordinte;

out vec3 pixel;

uniform sampler2D frame_buffer;

void main() {
	pixel = texture(frame_buffer, tex_coordinte).xyz;
}