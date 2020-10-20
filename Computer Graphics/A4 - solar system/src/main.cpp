#include "cgmath.h"
#include "cgut.h"
#include "trackball.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static const char*	window_name = "Solar System";
static const char*	vert_shader_path = "../bin/shaders/Solar_System.vert";
static const char*	frag_shader_path = "../bin/shaders/Solar_System.frag";
static const char** texture_path;


GLFWwindow*	window = nullptr;
ivec2		window_size = ivec2(1024, 576);

GLuint		program = 0;
GLuint		vertex_buffer = 0;
GLuint		index_buffer = 0;
GLuint		vertex_buffer2 = 0;
GLuint		index_buffer2 = 0;

struct camera
{
	vec3	eye = vec3(100, 0, 0);
	vec3	at = vec3(0, 0, 0);
	vec3	up = vec3(0, 0, 1);
	mat4	view_matrix = mat4::look_at(eye, at, up);

	float	fovy = PI / 4.0f;
	float	aspect_ratio = window_size.x / (float)window_size.y;
	float	dnear = 1.0f;
	float	dfar = 10000.0f;
	mat4	projection_matrix = mat4::perspective(fovy, aspect_ratio, dnear, dfar);
};

struct light_t
{
	vec4	position = vec4(0.0f, 0.0f, 0.0f, 1.0f);  
	vec4	ambient = vec4(0.2f, 0.2f, 0.2f, 1.0f);
	vec4	diffuse = vec4(0.8f, 0.8f, 0.8f, 1.0f);
	vec4	specular = vec4(1.0f, 1.0f, 1.0f, 1.0f);
};

struct material_t
{
	GLuint diffuseTexture;
	GLuint normalTexture; 
	vec4 ambient = vec4(0.2f, 0.2f, 0.2f, 1.0f);
	vec4 specular = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	float shininess = 1000.0f;
};

struct planet
{
	float	radius;
	mat4	model_matrix;
	float	rot;
	float	rev;
	float	trans;
	float	theta1;	
	float	theta2;
	uint	dnum;
};

int		frame = 0;
bool	bwire = false;
bool	bpause = false;
float	t;
float	lt = 0;
float   ct, pt;
uint	Num_longit = 36;
uint	Num_latit = 72;

std::vector<vertex> vertices;
std::vector<uint> indices;
std::vector<vertex> vertices2;
std::vector<uint> indices2;
std::vector<planet> planets;
std::vector<planet> dwarfs;
std::vector<material_t> tex;
camera C;
mat4 model_matrix;
trackball	tb;
light_t light;
material_t dtex;
material_t r1tex;
material_t r2tex;

extern "C" unsigned char* stbi_load(const char* filename, int* x, int* y, int* comp, int req_comp);
extern "C" void stbi_image_free(void* retval_from_stbi_load);

void update()
{
	ct = float(glfwGetTime());
	pt = ct - lt;
	lt = ct;
	t = pt;
	
	C.aspect_ratio = window_size.x / (float)window_size.y;
	C.projection_matrix = mat4::perspective(C.fovy, C.aspect_ratio, C.dnear, C.dfar);

	GLint uloc;
	uloc = glGetUniformLocation(program, "view_matrix");			if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, C.view_matrix * tb.tbrot_stack * tb.tbrot_matrix);
	uloc = glGetUniformLocation(program, "projection_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, C.projection_matrix);
	glUniform4fv(glGetUniformLocation(program, "light_position"), 1, light.position);
	glUniform4fv(glGetUniformLocation(program, "Ia"), 1, light.ambient);
	glUniform4fv(glGetUniformLocation(program, "Id"), 1, light.diffuse);
	glUniform4fv(glGetUniformLocation(program, "Is"), 1, light.specular);
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(program);
	const char*	vertex_attrib[] = { "position", "normal", "texcoord" };
	size_t		attrib_size[] = { sizeof(vertex().pos), sizeof(vertex().norm), sizeof(vertex().tex) };
	for (size_t k = 0, kn = std::extent<decltype(vertex_attrib)>::value, byte_offset = 0; k < kn; k++, byte_offset += attrib_size[k - 1])
	{
		GLuint loc = glGetAttribLocation(program, vertex_attrib[k]); if (loc >= kn) continue;
		glEnableVertexAttribArray(loc);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
		glVertexAttribPointer(loc, attrib_size[k] / sizeof(GLfloat), GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid*)byte_offset);
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);

	GLint uloc;
	uint dwf = 0;
	for (int i = 0; i < 9; i++) {
		glUniform1i(glGetUniformLocation(program, "TEXd"), tex[i].diffuseTexture);
		glUniform1i(glGetUniformLocation(program, "TEXn"), tex[i].normalTexture);
		glUniform1f(glGetUniformLocation(program, "shininess"), tex[i].shininess);
		glUniform4fv(glGetUniformLocation(program, "Ka"), 1, tex[i].ambient);
		glUniform4fv(glGetUniformLocation(program, "Ks"), 1, tex[i].specular);
		glActiveTexture(GL_TEXTURE0 + tex[i].diffuseTexture);
		glBindTexture(GL_TEXTURE_2D, tex[i].diffuseTexture);
		if (i == 1 || i == 2 || i == 3 || i == 4) {
			glActiveTexture(GL_TEXTURE0 + tex[i].normalTexture);
			glBindTexture(GL_TEXTURE_2D, tex[i].normalTexture);
			glUniform1f(glGetUniformLocation(program, "bnormal"), (float)1);
		}
		else glUniform1f(glGetUniformLocation(program, "bnormal"), (float)0);
		if (!bpause) {
			planets[i].theta1 += t * planets[i].rot;
			planets[i].theta2 += t * planets[i].rev;
		}
		planets[i].model_matrix = mat4::rotate(vec3(0, 0, 1), planets[i].theta2)  * mat4::translate(vec3(0,planets[i].trans,0)) * mat4::rotate(vec3(0,0,1), planets[i].theta1) * mat4::scale(vec3(planets[i].radius));
		uloc = glGetUniformLocation(program, "model_matrix");			if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, planets[i].model_matrix);
		if (i == 0) glUniform1f( glGetUniformLocation(program, "is_sun"), (float)1);
		else glUniform1f(glGetUniformLocation(program, "is_sun"), (float)0);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);

		glUniform1i(glGetUniformLocation(program, "TEXd"), dtex.diffuseTexture);
		glUniform1i(glGetUniformLocation(program, "TEXn"), dtex.normalTexture);
		glUniform1f(glGetUniformLocation(program, "shininess"), dtex.shininess);
		glUniform4fv(glGetUniformLocation(program, "Ka"), 1, dtex.ambient);
		glUniform4fv(glGetUniformLocation(program, "Ks"), 1, dtex.specular);
		glActiveTexture(GL_TEXTURE0 + dtex.diffuseTexture);
		glBindTexture(GL_TEXTURE_2D, dtex.diffuseTexture);
		glActiveTexture(GL_TEXTURE0 + dtex.normalTexture);
		glBindTexture(GL_TEXTURE_2D, dtex.normalTexture);
		glUniform1f(glGetUniformLocation(program, "bring"), (float)0);
		glUniform1f(glGetUniformLocation(program, "bnormal"), (float)1);
		for (uint j = dwf; j < dwf + planets[i].dnum; j++) {
			if (!bpause) {
				dwarfs[j].theta1 += t * dwarfs[j].rot;
				dwarfs[j].theta2 += t * dwarfs[j].rev;
			}
			dwarfs[j].model_matrix = mat4::rotate(vec3(0, 0, 1), planets[i].theta2)  * mat4::translate(vec3(0, planets[i].trans, 0)) * mat4::rotate(vec3(0, 0, 1), planets[i].theta1)
				* mat4::rotate(vec3(0, 0, 1), dwarfs[j].theta2) * mat4::translate(vec3(0, dwarfs[j].trans, 0)) * mat4::rotate(vec3(0, 0, 1), dwarfs[j].theta1) * mat4::scale(vec3(dwarfs[j].radius));
			uloc = glGetUniformLocation(program, "model_matrix");			if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, dwarfs[j].model_matrix);
			glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
		}
		dwf += planets[i].dnum;
	}
	for (size_t k = 0, kn = std::extent<decltype(vertex_attrib)>::value, byte_offset = 0; k < kn; k++, byte_offset += attrib_size[k - 1])
	{
		GLuint loc = glGetAttribLocation(program, vertex_attrib[k]); if (loc >= kn) continue;
		glEnableVertexAttribArray(loc);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer2);
		glVertexAttribPointer(loc, attrib_size[k] / sizeof(GLfloat), GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid*)byte_offset);
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer2);
	glDisable(GL_CULL_FACE);
	uloc = glGetUniformLocation(program, "model_matrix");			if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, planets[6].model_matrix);
	glUniform1i(glGetUniformLocation(program, "TEXd"), r1tex.diffuseTexture);
	glUniform1i(glGetUniformLocation(program, "TEXn"), r1tex.normalTexture);
	glUniform1f(glGetUniformLocation(program, "shininess"), r1tex.shininess);
	glUniform4fv(glGetUniformLocation(program, "Ka"), 1, r1tex.ambient);
	glUniform4fv(glGetUniformLocation(program, "Ks"), 1, r1tex.specular);
	glActiveTexture(GL_TEXTURE0 + r1tex.diffuseTexture);
	glBindTexture(GL_TEXTURE_2D, r1tex.diffuseTexture);
	glActiveTexture(GL_TEXTURE0 + r1tex.normalTexture);
	glBindTexture(GL_TEXTURE_2D, r1tex.normalTexture);
	glUniform1f(glGetUniformLocation(program, "bnormal"), (float)0);
	glUniform1f(glGetUniformLocation(program, "bring"), (float)1);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
	uloc = glGetUniformLocation(program, "model_matrix");			if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, planets[7].model_matrix);
	glUniform1i(glGetUniformLocation(program, "TEXd"), r2tex.diffuseTexture);
	glUniform1i(glGetUniformLocation(program, "TEXn"), r2tex.normalTexture);
	glUniform1f(glGetUniformLocation(program, "shininess"), r2tex.shininess);
	glUniform4fv(glGetUniformLocation(program, "Ka"), 1, r2tex.ambient);
	glUniform4fv(glGetUniformLocation(program, "Ks"), 1, r2tex.specular);
	glActiveTexture(GL_TEXTURE0 + r2tex.diffuseTexture);
	glBindTexture(GL_TEXTURE_2D, r2tex.diffuseTexture);
	glActiveTexture(GL_TEXTURE0 + r2tex.normalTexture);
	glBindTexture(GL_TEXTURE_2D, r2tex.normalTexture);
	glUniform1f(glGetUniformLocation(program, "bnormal"), (float)0);
	glUniform1f(glGetUniformLocation(program, "bring"), (float)1);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
	glEnable(GL_CULL_FACE);
	glfwSwapBuffers(window);
}

void reshape(GLFWwindow* window, int width, int height)
{
	window_size = ivec2(width, height);
	glViewport(0, 0, width, height);
}

void print_help()
{
	printf("[help]\n");
	printf("- press ESC or 'q' to terminate the program\n");
	printf("- press F1 or 'h' to see help\n");
	printf("- press 'w' to toggle between Wireframe_Mode and Solid_Mode\n");
	printf("- press 'Home' to reset camera\n");
	printf("- press 'Pause' to pause the simulation\n");
	printf("- use left button of mouse to rotating camera\n");
	printf("- use right button or shift+left button of mouse to zooming camera\n");
	printf("- use middle button or ctrl+left button of mouse to panning camera\n");
}

void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q)	glfwSetWindowShouldClose(window, GL_TRUE);
		else if (key == GLFW_KEY_H || key == GLFW_KEY_F1)	print_help();
		else if (key == GLFW_KEY_W)
		{
			bwire = !bwire;
			glPolygonMode(GL_FRONT_AND_BACK, bwire ? GL_LINE : GL_FILL);
			printf("Using %s Mode\n", bwire ? "wireframe" : "solid");
		}
		else if (key == GLFW_KEY_HOME)
		{
			C.eye = vec3(100, 0, 0);
			C.at = vec3(0, 0, 0);
			C.up = vec3(0, 0, 1);
			tb.tbrot_stack= mat4::identity();
			tb.tbrot_matrix = mat4::identity();
			C.view_matrix = mat4::look_at(C.eye, C.at, C.up);
			printf("Reset camera finished\n");
		}
		else if (key == GLFW_KEY_PAUSE)
		{
			bpause = !bpause;
			printf("%s the simulation\n", bpause ? "Pause" : "Resume");
		}
	}
}

void mouse(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT || button == GLFW_MOUSE_BUTTON_RIGHT || button == GLFW_MOUSE_BUTTON_MIDDLE )
	{
		dvec2 pos; glfwGetCursorPos(window, &pos.x, &pos.y);
		vec2 npos = vec2(float(pos.x) / float(window_size.x - 1), float(pos.y) / float(window_size.y - 1));
		if (action == GLFW_PRESS)			tb.begin(C.view_matrix, npos.x, npos.y, C.eye, C.at, C.up, C.fovy ,C.aspect_ratio);
		else if (action == GLFW_RELEASE)	tb.end();
		tb.button = button;
		tb.mods = mods;
	}
}

void motion(GLFWwindow* window, double x, double y)
{
	if (!tb.is_tracking()) return;
	if (tb.button == GLFW_MOUSE_BUTTON_LEFT && tb.mods == 0) {
		vec2 npos = vec2(float(x) / float(window_size.x - 1), float(y) / float(window_size.y - 1));
		tb.update_rot(npos.x, npos.y);
	}
	else if (tb.button == GLFW_MOUSE_BUTTON_MIDDLE || (tb.button == GLFW_MOUSE_BUTTON_LEFT && (tb.mods&GLFW_MOD_CONTROL))) {
		vec2 npos = vec2(float(x) / float(window_size.x - 1), float(y) / float(window_size.y - 1));
		tb.update_pan(npos.x, npos.y);
		C.eye = tb.eye;
		C.at = tb.at;
		C.view_matrix = mat4::look_at(C.eye, C.at, C.up);
	}
	else if (tb.button == GLFW_MOUSE_BUTTON_RIGHT || (tb.button == GLFW_MOUSE_BUTTON_LEFT && (tb.mods&GLFW_MOD_SHIFT))) {
		vec2 npos = vec2(float(x) / float(window_size.x - 1), float(y) / float(window_size.y - 1));
		tb.update_zoom(npos.x, npos.y);
		C.eye = tb.eye;
		C.view_matrix = mat4::look_at(C.eye, C.at, C.up);
	}
}

void Set_vertices(uint n, uint m)
{
	vertices.clear();
	for (uint i = 0; i <= n; i++) {
		float theta = PI / (float)n * (float)i;
		for (uint j = 0; j <= m; j++) {
			float phi = PI * 2.0f / (float)m * (float)j;
			vertices.push_back({ vec3(sin(theta) * cos(phi), sin(theta) * sin(phi) , cos(theta)), vec3(sin(theta) * cos(phi), sin(theta) * sin(phi) , cos(theta)), vec2(phi / (2 * PI), 1 - (theta / PI)) });
		}
	}
}

void Set_Buffer(uint n, uint m)
{
	if (vertex_buffer)	glDeleteBuffers(1, &vertex_buffer);	vertex_buffer = 0;
	if (index_buffer)	glDeleteBuffers(1, &index_buffer);	index_buffer = 0;
	if (vertices.empty()) { printf("vertices Array is empty.\n"); return; }
	indices.clear();
	for (uint i = 0; i < n; i++) {
		for (uint j = 0; j < m; j++) {
			if (i == 0) {
				indices.push_back(j);
				indices.push_back((m + 1) * (i + 1) + j);
				indices.push_back((m + 1) * (i + 1) + j + 1);
			}
			else if (i == n - 1) {
				indices.push_back((m + 1) * i  + j);
				indices.push_back((m + 1) * (i + 1) + j);
				indices.push_back((m + 1) * i  + j + 1);
			}
			else {
				indices.push_back((m + 1) * i  + j);
				indices.push_back((m + 1) * (i + 1) + j);
				indices.push_back((m + 1) * i + j + 1);
				indices.push_back((m + 1) * i + j + 1);
				indices.push_back((m + 1) * (i + 1) + j);
				indices.push_back((m + 1) * (i + 1) + j + 1);
			}
		}
	}
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex)*vertices.size(), &vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint)*indices.size(), &indices[0], GL_STATIC_DRAW);
}

void Set_Ring(uint n)
{
	vertices2.clear();
	for (uint i = 0; i <= n; i++)
	{
		float t = PI * 2.0f / float(n) * float(i);
		float c = cos(t), s = sin(t);
		vertices2.push_back({ vec3(1.3f * c,1.3f * s,0.0f), vec3(1.3f * c,1.3f * s,0.0f), vec2(1,1) });
	}
	for (uint i = 0; i <= n; i++)
	{
		float t = PI * 2.0f / float(n) * float(i);
		float c = cos(t), s = sin(t);
		vertices2.push_back({ vec3(3.0f*c,3.0f*s,0.0f), vec3(3.0f * c,3.0f * s,0.0f), vec2(0,0) });
	}

	if (vertex_buffer2)	glDeleteBuffers(1, &vertex_buffer2);	vertex_buffer2 = 0;
	if (index_buffer2)	glDeleteBuffers(1, &index_buffer2);	index_buffer2 = 0;
	if (vertices2.empty()) { printf("vertices Array is empty.\n"); return; }
	indices2.clear();
	for (uint i = 0; i < n; i++) {
		indices2.push_back(i);
		indices2.push_back(i + n + 1);
		indices2.push_back(i + 1);
		indices2.push_back(i + 1);
		indices2.push_back(i + n + 1);
		indices2.push_back(i + n + 2);
	}
	glGenBuffers(1, &vertex_buffer2);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex)*vertices2.size(), &vertices2[0], GL_STATIC_DRAW);
	glGenBuffers(1, &index_buffer2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint)*indices2.size(), &indices2[0], GL_STATIC_DRAW);
}

void Set_Planets()
{
	planets.clear();	
	planets.push_back({10.0f, mat4::mat4(), 0.8f,  0.0f,  0.0f, 0.0f, 0.0f, 0});
	planets.push_back({ 2.0f, mat4::mat4(), 0.3f, 3.8f, 20.0f, 0.0f, (2*PI * 5) / 8, 0 });
	planets.push_back({ 3.0f, mat4::mat4(), 0.1f, 2.3f, 30.0f, 0.0f, (2*PI * 1) / 8, 0 });
	planets.push_back({ 3.5f, mat4::mat4(), 1.0f, 0.8f, 45.0f, 0.0f, (2*PI * 6) / 8, 1 });
	planets.push_back({ 2.5f, mat4::mat4(), 0.9f, 0.5f, 67.0f, 0.0f, (2*PI * 8) / 8, 0 });
	planets.push_back({ 8.0f, mat4::mat4(), 2.5f, 1.2f, 94.0f, 0.0f, (2*PI * 2) / 8, 4 });
	planets.push_back({ 6.0f, mat4::mat4(), 2.1f, 0.7f, 135.0f, 0.0f, (2*PI * 4) / 8, 0 });
	planets.push_back({ 5.0f, mat4::mat4(), 1.6f, 1.0f, 170.0f, 0.0f, (2*PI * 7) / 8, 2 });
	planets.push_back({ 4.0f, mat4::mat4(), 1.7f, 1.2f, 200.0f, 0.0f, (2*PI * 3) / 8, 5 });
	dwarfs.clear();
	dwarfs.push_back({ 1.0f, mat4::mat4(), 0.5f,  0.6f,  5.5f, 0.0f, 0.0f, 0 });
	dwarfs.push_back({ 2.0f, mat4::mat4(), 1.0f,  1.8f,  11.0f, 0.0f, (2 * PI * 3) / 4, 0 });
	dwarfs.push_back({ 1.7f, mat4::mat4(), 1.5f,  0.5f,  14.0f, 0.0f, (2 * PI * 1) / 4, 0 });
	dwarfs.push_back({ 2.2f, mat4::mat4(), 2.0f,  1.0f,  17.0f, 0.0f, (2 * PI * 2) / 4, 0 });
	dwarfs.push_back({ 1.5f, mat4::mat4(), 0.7f,  0.2f,  20.0f, 0.0f, (2 * PI * 4) / 4, 0 });
	dwarfs.push_back({ 1.2f, mat4::mat4(), 0.7f,  1.5f,  7.0f, 0.0f, PI, 0 });
	dwarfs.push_back({ 1.5f, mat4::mat4(), 1.5f,  0.7f,  10.0f, 0.0f, 0.0f, 0 });
	dwarfs.push_back({ 1.0f, mat4::mat4(), 1.2f,  0.5f,  5.5f, 0.0f, (2 * PI * 2) / 5, 0 });
	dwarfs.push_back({ 0.7f, mat4::mat4(), 0.5f,  1.2f,  7.5f, 0.0f, (2 * PI * 5) / 5, 0 });
	dwarfs.push_back({ 1.2f, mat4::mat4(), 0.3f,  1.5f,  9.7f, 0.0f, (2 * PI * 3) / 5, 0 });
	dwarfs.push_back({ 1.1f, mat4::mat4(), 1.5f,  0.7f,  12.5f, 0.0f, (2 * PI * 1) / 5, 0 });
	dwarfs.push_back({ 0.8f, mat4::mat4(), 0.7f,  0.3f,  15.0f, 0.0f, (2 * PI * 4) / 5, 0 });
}

void Set_tex()
{
	int width, height, comp = 3;
	unsigned char* pimage0;
	unsigned char* pimage;
	int stride0, stride1;
	GLuint tmp[19];

	for (int i = 0; i < 19; i++) {
		pimage0 = stbi_load(texture_path[i], &width, &height, &comp, 3); if (comp == 1) comp = 3;
		stride0 = width * comp;
		stride1 = (stride0 + 3)&(~3);
		pimage = (unsigned char*)malloc(sizeof(unsigned char)*stride1*height);
		for (int y = 0; y < height; y++) memcpy(pimage + (height - 1 - y)*stride1, pimage0 + y * stride0, stride0);
		glGenTextures(1, &tmp[i]);
		glBindTexture(GL_TEXTURE_2D, tmp[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pimage);
		int mip_levels = miplevels(window_size.x, window_size.y);
		for (int k = 1, w = width >> 1, h = height >> 1; k < mip_levels; k++, w = max(1, w >> 1), h = max(1, h >> 1))
			glTexImage2D(GL_TEXTURE_2D, k, GL_RGB8, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	int cnt = 0;
	for (int i = 0; i < 9; i++) {
		if (i == 1 || i == 2 || i == 3 || i == 4) {
			tex.push_back({ tmp[cnt], tmp[cnt + 1]});
			cnt += 2;
		}
		else {
			tex.push_back({ tmp[cnt] });
			cnt++;
		}
	}
	dtex.diffuseTexture = tmp[cnt++];
	dtex.normalTexture = tmp[cnt++];
	r1tex.diffuseTexture = tmp[cnt++];
	r1tex.normalTexture = tmp[cnt++];
	r2tex.diffuseTexture = tmp[cnt++];
	r2tex.normalTexture = tmp[cnt++];
}
bool user_init()
{
	texture_path = (const char**)malloc(sizeof(const char*) * 19);
	texture_path[0] = "../bin/textures/sun.jpg";
	texture_path[1] = "../bin/textures/mercury.jpg";
	texture_path[2] = "../bin/textures/mercury-normal.jpg";
	texture_path[3] = "../bin/textures/venus.jpg";
	texture_path[4] = "../bin/textures/venus-normal.jpg";
	texture_path[5] = "../bin/textures/earth.jpg";
	texture_path[6] = "../bin/textures/earth-normal.jpg";
	texture_path[7] = "../bin/textures/mars.jpg";
	texture_path[8] = "../bin/textures/mars-normal.jpg";
	texture_path[9] = "../bin/textures/jupiter.jpg";
	texture_path[10] = "../bin/textures/saturn.jpg";
	texture_path[11] = "../bin/textures/uranus.jpg";
	texture_path[12] = "../bin/textures/neptune.jpg";
	texture_path[13] = "../bin/textures/moon.jpg";
	texture_path[14] = "../bin/textures/moon-normal.jpg";
	texture_path[15] = "../bin/textures/saturn-ring.jpg";
	texture_path[16] = "../bin/textures/saturn-ring-alpha.jpg";
	texture_path[17] = "../bin/textures/uranus-ring.jpg";
	texture_path[18] = "../bin/textures/uranus-ring-alpha.jpg";

	print_help();
	glClearColor(39 / 255.0f, 40 / 255.0f, 34 / 255.0f, 1.0f);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	Set_vertices(Num_longit, Num_latit);
	Set_Buffer(Num_longit, Num_latit);
	Set_Ring(Num_latit);
	Set_Planets();
	Set_tex();
	return true;
}

void user_finalize()
{
}

int main(int argc, char* argv[])
{
	if (!glfwInit()) { printf("[error] failed in glfwInit()\n"); return 1; }

	if (!(window = cg_create_window(window_name, window_size.x, window_size.y))) { glfwTerminate(); return 1; }
	if (!cg_init_extensions(window)) { glfwTerminate(); return 1; }

	if (!(program = cg_create_program(vert_shader_path, frag_shader_path))) { glfwTerminate(); return 1; }
	if (!user_init()) { printf("Failed to user_init()\n"); glfwTerminate(); return 1; }

	glfwSetWindowSizeCallback(window, reshape);
	glfwSetKeyCallback(window, keyboard);
	glfwSetMouseButtonCallback(window, mouse);
	glfwSetCursorPosCallback(window, motion);

	for (frame = 0; !glfwWindowShouldClose(window); frame++)
	{
		glfwPollEvents();
		update();
		render();
	}

	user_finalize();
	cg_destroy_window(window);

	return 0;
}