#version 130

in vec3 position;
in vec3 normal;
in vec2 texcoord;	// color

out vec2 tc;

uniform float	ratio;
uniform mat4 model_matrix;
uniform bool brot;
uniform mat4 view_projection_matrix;

void main()
{
	gl_Position = view_projection_matrix * model_matrix * vec4( position, 1 );
	gl_Position.xy *= ratio > 1 ? vec2(1 / ratio, 1) : vec2(1, ratio);
	tc = texcoord;
}