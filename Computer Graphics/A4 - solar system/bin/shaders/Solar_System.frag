#version 130

in vec4 epos;
in vec3 norm;
in vec2 tc;
in mat3 TBN;

out vec4 fragColor;

uniform float tex;
uniform mat4 view_matrix;
uniform vec4 light_position, Ia, Id, Is;
uniform float shininess;
uniform float is_sun;
uniform float bnormal;
uniform float bring;
uniform vec4 Ka;
uniform vec4 Ks; 
uniform sampler2D TEXd;
uniform sampler2D TEXn;

void main()
{
	vec4 Kd = texture2D( TEXd, tc );
	if(is_sun==1) 
		fragColor = Kd;
	else{
		vec3 normal;
		if(bnormal==1){
			normal = 2.0 * texture2D(TEXn, tc).rgb - 1.0;
			normal = normalize(TBN*normal);
		}
		else {
			normal = norm;
		}
		vec4 lpos = view_matrix*light_position;
		vec3 n = normalize(normal);
		vec3 p = epos.xyz;
		vec3 l = normalize(lpos.xyz-(lpos.a==0.0?vec3(0):p));
		vec3 v = normalize(-p);
		vec3 h = normalize(l+v);
		vec4 Ira = Ka*Ia;
		vec4 Ird = max(Kd*dot(l,n)*Id,0.0);
		vec4 Irs = max(Ks*pow(dot(h,n),shininess)*Is,0.0);
		fragColor = Ira + Ird + Irs;
		if(bring==1){
		fragColor.a = texture2D( TEXn, tc ).r;
		}
	}
}
