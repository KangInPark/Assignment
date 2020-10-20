#ifndef __TRACKBALL_H__
#define __TRACKBALL_H__
#include "cgmath.h"

struct trackball
{
	bool	b_tracking = false;
	float	scale;			
	mat4	view_matrix0;	
	vec2	m0;				
	int		button;
	int		mods;
	mat4	tbrot_matrix;
	mat4	tbrot_stack;
	vec3	eye;
	vec3	at;
	vec3	up;
	vec3	eye0;
	vec3	at0;
	vec3	up0;
	float	fovy0;
	float	aspectratio0;

	trackball( float rot_scale=1.0f ):scale(rot_scale){}
	bool is_tracking() const { return b_tracking; }
	void begin( const mat4& view_matrix, float x, float y , vec3 e, vec3 a, vec3 u, float fovy, float aspectratio)
	{
		b_tracking = true;			
		m0 = vec2(x,y)*2.0f-1.0f;	
		tbrot_stack = tbrot_stack * tbrot_matrix;
		tbrot_matrix = mat4::identity();
		view_matrix0 = view_matrix * tbrot_stack;	
		eye0 = e;
		at0 = a;
		up0 = u;
		fovy0 = fovy;
		aspectratio0 = aspectratio;

	}
	void end() { b_tracking = false; }


	void update_rot( float x, float y )
	{
		vec2 m = vec2(x,y)*2.0f - 1.0f; 

		static const vec3 p0 = vec3(0,0,1.0f);	
		vec3 p1 = vec3(m.x-m0.x, m0.y-m.y,0);	
		if (!b_tracking || length(p1) < 0.0001f) return;
		p1 *= scale;														
		p1 = vec3(p1.x,p1.y,sqrtf(max(0,1.0f-length2(p1)))).normalize();	

		vec3 n = p0.cross(p1)*mat3(view_matrix0);
		float angle = asin( min(n.length(),0.999f) );

		tbrot_matrix = mat4::rotate(n.normalize(), angle);
	}

	void update_zoom(float x, float y)
	{
		vec2 m = vec2(x, y)*2.0f - 1.0f;
		eye = vec3(eye0.x * (max(-0.7f,(((m0.y - m.y) / 2.0f))) + 1), eye0.y, eye0.z);
	}

	void update_pan(float x, float y)
	{
		vec2 m = vec2(x, y)*2.0f - 1.0f;
		float dx = -((m.x - m0.x) / 2.0f);
		float dy = ((m.y - m0.y) / 2.0f);
		float disy = 2 * eye0.x * tan(fovy0 / 2);
		float disx = 2 * eye0.x * tan(fovy0 / 2) * aspectratio0;
		eye = vec3(eye0.x, eye0.y + disx * dx, eye0.z + disy * dy);
		at = vec3(at0.x, at0.y + disx * dx, at0.z + disy * dy);
	}
};

#endif // __TRACKBALL_H__
