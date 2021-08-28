#version 460 core
out vec3 pixel;

void main() {
	pixel = vec3(0.2 + gl_FragCoord.z, 0.4, 0.7);
}