#version 460 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 tex_coordinates;

uniform float rotation;

out vec3 vertex_color;
out vec2 tex_coord;

uniform mat4 transformation = mat4(1);

void main() {
//	vec2 rotated_position = position.xy * mat2(cos(rotation), -sin(rotation), sin(rotation), cos(rotation));
	gl_Position = transformation * vec4(position, 1.0);

	vertex_color = color;
	tex_coord = tex_coordinates;
}