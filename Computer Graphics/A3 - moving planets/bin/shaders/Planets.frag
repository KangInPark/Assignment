#version 130

in vec2 tc;

out vec4 fragColor;

uniform float tex;

void main()
{
	if(tex == 0)fragColor = vec4(tc.xy,0,1);
	else if(tex == 1)fragColor = vec4(tc.xxx,1);
	else fragColor = vec4(tc.yyy,1);
}
