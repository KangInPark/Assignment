#include "cgmath.h"
#include "cgut.h"

static const char*	window_name = "Sphere";
static const char*	vert_shader_path = "../bin/shaders/sphere.vert";
static const char*	frag_shader_path = "../bin/shaders/sphere.frag";

GLFWwindow*	window = nullptr;
ivec2		window_size = ivec2(1024, 576);

GLuint		program = 0;
GLuint		vertex_buffer = 0;
GLuint		index_buffer = 0;

int		frame = 0;
bool	bwire = false;
uint	tex = 0;
uint	rot = 0;
bool	brot = false;
float	ratio = window_size.x / (float)window_size.y;
float	t;
float	lt = 0;
float   ct, pt;
uint	Num_longit = 36;
uint	Num_latit = 72;
std::vector<vertex> vertices;
std::vector<uint> indices;

mat4 view_projection_matrix;
mat4		model_matrix;

void update()
{
	ct = float(glfwGetTime());
	pt = ct - lt;
	lt = ct;
	if(brot) t += pt*0.5f;
	switch (rot)
	{
		case 0:
			model_matrix =
			{
				1, 0, 0, 0,
				0, cosf(t), -sinf(t), 0,
				0, sinf(t), cosf(t), 0,
				0, 0, 0, 1
			};
			break;
		case 1:
			model_matrix =
			{
				cosf(t), 0, sinf(t), 0,
				0, 1, 0, 0,
				-sinf(t), 0, cosf(t), 0,
				0, 0, 0, 1
			};
			break;
		case 2:
			model_matrix =
			{ 
				cosf(t), -sinf(t), 0, 0,
				sinf(t), cosf(t), 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1
			};
			break;
	}
	GLint uloc;
	view_projection_matrix =
	{
		0,1,0,0,
		0,0,1,0,
		-1,0,0,1,
		0,0,0,1
	};
	ratio = window_size.x / (float)window_size.y;
	uloc = glGetUniformLocation(program, "ratio");					if (uloc > -1) glUniform1f(uloc, ratio);
	uloc = glGetUniformLocation(program, "tex");					if (uloc > -1) glUniform1f(uloc, (float)tex);
	uloc = glGetUniformLocation(program, "brot");					if (uloc > -1) glUniform1i(uloc, brot);
	uloc = glGetUniformLocation(program, "model_matrix");			if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix);
	uloc = glGetUniformLocation(program, "view_projection_matrix"); if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, view_projection_matrix);
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
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
	
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
	printf("- press 'r' to rotate the sphere\n");
	printf("- press 'e' to toggle rotate axes (X-axes) > (Y-axes) > (Z-axes)\n");
	printf("- press 't' to reset rotate\n");
	printf("\n");
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
		else if (key == GLFW_KEY_R)
		{
			brot = !brot;
			printf("Rotate %s\n", brot ? "On" : "Off");
		}
		else if (key == GLFW_KEY_E)
		{
			rot++;
			if (rot > 2)rot = 0;
			if (rot == 0) printf("Using (X-axes) rotation\n");
			else if (rot == 1) printf("Using (Y-axes) rotation\n");
			else printf("Using (Z-axes) rotation\n");
		}
		else if (key == GLFW_KEY_T)
		{
			t = 0;
			printf("Reset finished\n");
		}
	}
}

void mouse(GLFWwindow* window, int button, int action, int mods)
{
}

void motion(GLFWwindow* window, double x, double y)
{
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

bool user_init()
{
	print_help();
	glClearColor(39 / 255.0f, 40 / 255.0f, 34 / 255.0f, 1.0f);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);

	Set_vertices(Num_longit, Num_latit);
	Set_Buffer(Num_longit, Num_latit);

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