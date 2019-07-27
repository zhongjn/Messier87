#version 440
uniform sampler2D tex_in;
uniform vec3 resolution;

void main() {
	gl_FragColor = texture(tex_in, gl_FragCoord.xy / resolution.xy);
}