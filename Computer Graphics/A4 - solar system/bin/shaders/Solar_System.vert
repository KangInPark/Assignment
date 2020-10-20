#version 130

in vec3 position;
in vec3 normal;
in vec2 texcoord;	// color

out vec3 norm;
out vec4 epos;
out vec2 tc;
out mat3 TBN;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

void main()
{
	norm = normalize(mat3(view_matrix*model_matrix)*normal);
	vec4 wpos = model_matrix * vec4(position,1);
	epos = view_matrix * wpos;
	gl_Position = projection_matrix * epos;
	tc = texcoord;

	vec3 tangent; 
	vec3 binormal;
	vec3 c1 = cross(norm, vec3(0.0, 0.0, 1.0)); 
	vec3 c2 = cross(norm, vec3(0.0, 1.0, 0.0)); 
	if(length(c1)>length(c2))
	{
		tangent = c1;	
	}
	else
	{
		tangent = c2;	
	}
	tangent = normalize(tangent);
	binormal = cross(norm, tangent); 
	binormal = normalize(binormal);
	TBN = mat3(tangent,binormal,norm);
}