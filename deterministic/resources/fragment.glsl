#version 460 core
in vec3 vertex_color;
in vec2 tex_coord;

uniform float factor = 1.0;
uniform sampler2D wall_texture;

out vec3 pixel;

void main() {
//	const float r = (sin(gl_FragCoord.x / factor) + 1) / 2;
//	const float g = (sin(gl_FragCoord.y / factor) + 1) / 2;
//	const float b = (r + g) > 1 ? 0.5 : (r + g);
//	pixel = vec3(r, g, b);
	vec3 a = texture(wall_texture, tex_coord).xyz;
	vec3 b = vertex_color;
	pixel = mix(a, b, factor);
}