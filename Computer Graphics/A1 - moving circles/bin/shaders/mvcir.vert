#version 130

in vec3 position;
in vec3 normal;
in vec2 texcoord;	// color

out vec3 norm;
out vec2 tc;

uniform float	radius;
uniform float	ratio;
uniform float	ratioi;
uniform vec2	pos;

void main()
{
	gl_Position = vec4( position * radius, 1 );
	gl_Position.xy += pos;
	gl_Position.xy *= ratio>ratioi ? vec2(ratioi / ratio, 1) : vec2(1, ratio/ratioi);
	gl_Position.xy *= ratioi > 1 ? vec2(1 / ratioi, 1) : vec2(1, ratioi);

	norm = normal;
	tc = texcoord;
}