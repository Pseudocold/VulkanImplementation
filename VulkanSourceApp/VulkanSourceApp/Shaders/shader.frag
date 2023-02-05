#version 450			// Use GLSL 4.5

layout(location = 0) in vec3 fragCol;

layout(location = 0) out vec4 outColor; //final output color (must also have location)

void main() {
	outColor = vec4(fragCol, 1.0);
}