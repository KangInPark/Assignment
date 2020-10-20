#include "cgmath.h"
#include "cgut.h"
#include "trackball.h"

static const char*	window_name = "Moving Planets";
static const char*	vert_shader_path = "../bin/shaders/Planets.vert";
static const char*	frag_shader_path = "../bin/shaders/Planets.frag";

GLFWwindow*	window = nullptr;
ivec2		window_size = ivec2(1024, 576);

GLuint		program = 0;
GLuint		vertex_buffer = 0;
GLuint		index_buffer = 0;

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

struct planet
{
	float	radius;
	mat4	model_matrix;
	float	rot;
	float	rev;
	float	trans;
	float	theta1;
	float	theta2;
};

int		frame = 0;
bool	bwire = false;
bool	bpause = false;
uint	tex = 0;
float	t;
float	lt = 0;
float   ct, pt;
uint	Num_longit = 36;
uint	Num_latit = 72;

std::vector<vertex> vertices;
std::vector<uint> indices;
std::vector<planet> planets; 

camera C;
mat4 model_matrix;
trackball	tb;

void update()
{
	ct = float(glfwGetTime());
	pt = ct - lt;
	lt = ct;
	t = pt;
	
	C.aspect_ratio = window_size.x / (float)window_size.y;
	C.projection_matrix = mat4::perspective(C.fovy, C.aspect_ratio, C.dnear, C.dfar);

	GLint uloc;
	uloc = glGetUniformLocation(program, "tex");					if (uloc > -1) glUniform1f(uloc, (float)tex);
	uloc = glGetUniformLocation(program, "view_matrix");			if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, C.view_matrix * tb.tbrot_stack * tb.tbrot_matrix);
	uloc = glGetUniformLocation(program, "projection_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, C.projection_matrix);
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
	for (int i = 0; i < 9; i++) { 
		if (!bpause) {
			planets[i].theta1 += t * planets[i].rot;
			planets[i].theta2 += t * planets[i].rev;
		}
		planets[i].model_matrix = mat4::rotate(vec3(0, 0, 1), planets[i].theta2)  * mat4::translate(vec3(0,planets[i].trans,0)) * mat4::rotate(vec3(0,0,1), planets[i].theta1) * mat4::scale(vec3(planets[i].radius));
		uloc = glGetUniformLocation(program, "model_matrix");			if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, planets[i].model_matrix);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
	}
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
	printf("- press 'd' to toggle (tc.xy,0) > (tc.xxx) > (tc.yyy)\n");
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
		else if (key == GLFW_KEY_D)
		{
			tex++;
			if (tex > 2)tex = 0;
			if (tex == 0) printf("Using (texcoord.xy,0) as color\n");
			else if (tex == 1) printf("Using (texcoord.xxx) as color\n");
			else printf("Using (texcoord.yyy) as color\n");
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

void Set_Planets()
{
	planets.clear();	
	planets.push_back({10.0f, mat4::mat4(), 0.8f,  0.0f,  0.0f, 0.0f, 0.0f});
	planets.push_back({ 2.0f, mat4::mat4(), 0.3f, 4.0f, 20.0f, 0.0f, (2*PI * 5) / 8 });
	planets.push_back({ 3.0f, mat4::mat4(), 0.1f, 2.5f, 30.0f, 0.0f, (2*PI * 1) / 8 });
	planets.push_back({ 3.5f, mat4::mat4(), 1.0f, 1.0f, 45.0f, 0.0f, (2*PI * 6) / 8 });
	planets.push_back({ 2.5f, mat4::mat4(), 0.9f, 0.5f, 67.0f, 0.0f, (2*PI * 8) / 8 });
	planets.push_back({ 8.0f, mat4::mat4(), 2.5f, 1.4f, 84.0f, 0.0f, (2*PI * 2) / 8 });
	planets.push_back({ 6.0f, mat4::mat4(), 2.1f, 0.3f, 110.0f, 0.0f, (2*PI * 4) / 8 });
	planets.push_back({ 5.0f, mat4::mat4(), 1.6f, 1.2f, 125.0f, 0.0f, (2*PI * 7) / 8 });
	planets.push_back({ 4.0f, mat4::mat4(), 1.7f, 2.0f, 140.0f, 0.0f, (2*PI * 3) / 8 });
}
bool user_init()
{
	print_help();
	glClearColor(39 / 255.0f, 40 / 255.0f, 34 / 255.0f, 1.0f);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);

	Set_vertices(Num_longit, Num_latit);
	Set_Buffer(Num_longit, Num_latit);
	Set_Planets();
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