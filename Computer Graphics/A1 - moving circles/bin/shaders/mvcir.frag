#version 130

in vec2 tc;

out vec4 fragColor;

uniform vec4 color;

void main()
{
	fragColor = color;
}
