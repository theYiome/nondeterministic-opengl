#version 460 core
out vec3 pixel;

void main() {
	pixel = vec3(gl_FragCoord.x / 500, gl_FragCoord.y / 500, 0.7);
}