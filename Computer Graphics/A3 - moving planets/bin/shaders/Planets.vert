#version 130

in vec3 position;
in vec3 normal;
in vec2 texcoord;	// color

out vec2 tc;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

void main()
{
	gl_Position = projection_matrix * view_matrix * model_matrix * vec4( position, 1 );
	tc = texcoord;
}