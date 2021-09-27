#version 460 core
in vec3 vertex_color;
in vec2 tex_coord;
in vec3 fragment_position;
in vec3 adjusted_normal;
in vec3 raw_normal;;

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

uniform Material material = {{1, 1, 1}, {1, 1, 1}, {1, 1, 1}, 32};

uniform vec3 camera_position = vec3(0, 0, 0);
uniform float specular_strength = 0.5;
uniform float specular_power = 8;

uniform vec3 light_position = vec3(0, 1, -5);
uniform vec3 light_color = vec3(1, 1, 1);
uniform float ambient_strength = 0.1;
uniform float light_strength = 4;

uniform float color_ratio = 0;

uniform samplerCube crate_texture;

out vec3 pixel;

vec3 calculate_light() {
	// ambient
	vec3 ambient = light_color * material.ambient;

	// diffuse
	vec3 norm = normalize(adjusted_normal);
	vec3 light_direction = normalize(light_position - fragment_position);
	float diffuse_factor = max(dot(norm, light_direction), 0);
	vec3 diffuse = light_color * (diffuse_factor * material.diffuse);

	// specular
//	vec3 camera_direction = normalize(light_position )

	return vec3(1, 1, 1);
}

void main() {

	vec3 a = texture(crate_texture, raw_normal).xyz;
	vec3 b = vertex_color;
	vec3 object_color = mix(a, b, color_ratio);

	vec3 norm = normalize(adjusted_normal);
	vec3 ambient = ambient_strength * light_color;

	vec3 diff = light_position - fragment_position;

	if(length(diff) < 0.3)
		pixel = light_color;
	else {
		vec3 light_dir = normalize(diff);
		float diffuse_strength = max(dot(norm, light_dir), 0);
		vec3 diffuse = diffuse_strength * light_color;

		vec3 camera_dir = normalize(camera_position - fragment_position);
		vec3 reflection_dir = reflect(-light_dir, norm);

		float spec = pow(max(dot(camera_dir, reflection_dir), 0), specular_power);

		float strenght = light_strength / length(diff);
		strenght = strenght > 1 ? 1 : strenght;
		pixel = object_color * (diffuse + ambient + specular_strength * spec) * strenght;
	}
}