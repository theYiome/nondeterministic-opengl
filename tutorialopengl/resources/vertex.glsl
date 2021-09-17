#version 460 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 tex_coordinates;
layout (location = 3) in vec3 normal_vec;

out vec3 vertex_color;
out vec2 tex_coord;
out vec3 fragment_position;
out vec3 normal;

// model matrix is a transformation matrix that:
// translates, scales and/or rotates
// your object to place it in the world at a location/orientation they belong to
uniform mat4 model = mat4(1);
uniform mat4 view = mat4(1);
uniform mat4 projection = mat4(1);

void main() {
	vec4 pos = vec4(position, 1.0);
	gl_Position = projection * view * model * pos;

	vertex_color = color;
	tex_coord = tex_coordinates;
	fragment_position = (model * pos).xyz;
	normal = mat3(transpose(inverse(model))) * normal_vec;
}