#include "cgmath.h"
#include "cgut.h"

static const char*	window_name = "Moving Circles";
static const char*	vert_shader_path = "../bin/shaders/mvcir.vert";
static const char*	frag_shader_path = "../bin/shaders/mvcir.frag";
static const uint	Num_cmax = 1000;
static const uint	Num_cmin = 1;

GLFWwindow*	window = nullptr;
ivec2		window_size = ivec2(1280, 720);

GLuint		program = 0;
GLuint		vertex_buffer = 0;
GLuint		index_buffer = 0;

int		frame = 0;
bool	bwire = false;
bool	brestart = true;
bool	bbasedonframe = false;
uint	Num_t = 96;
uint	Num_c = 30;
float	ratio = window_size.x / (float)window_size.y;
float	size_x;
float	size_y;
float	ratioi;
float	lastTime = -1, currentTime, passedTime;
float	base = -1;


struct circle {
	vec2 speed;
	float radius;
	vec2 ipos;
	vec4 color;
	float mass;
};

std::vector<vertex> vertices;
std::vector<uint> indices;
std::vector<circle> circles;

void cir_init();
bool chk_pos(float xpos, float ypos, uint n, float r);
void update()
{
	GLint uloc;
	ratio = window_size.x / (float)window_size.y;
	uloc = glGetUniformLocation(program, "ratio");				if (uloc > -1) glUniform1f(uloc, ratio);
	uloc = glGetUniformLocation(program, "ratioi");				if (uloc > -1) glUniform1f(uloc, ratioi);
	if (!bbasedonframe) {
		if (lastTime == -1) {
			lastTime = (float)glfwGetTime();
			passedTime = 0;
		}
		else {
			currentTime = (float)glfwGetTime();
			passedTime = currentTime - lastTime;
			lastTime = currentTime;
			if (passedTime > 0.05)base = -1;
			else base = passedTime / 0.5f;
		}
	}
}

void collision_chk(circle* tmp, uint n)
{
	float size_x = ratioi > 1 ? ratioi : 1;
	float size_y = ratioi > 1 ? 1 : 1 / ratioi;
	float axpos = tmp->ipos.x + tmp->speed.x * base;
	float aypos = tmp->ipos.y + tmp->speed.y * base;
	float bxpos, bypos;
	if (axpos > (size_x - tmp->radius) || axpos < (-1 * size_x + tmp->radius)) {
		if (aypos > (size_y - tmp->radius) || aypos < (-1 * size_y + tmp->radius)) {
			tmp->speed *= vec2(-1.0f, -1.0f);
			return;
		}
		tmp->speed *= vec2(-1.0f, 1.0f);
		return;
	}
	else if (aypos > (size_y - tmp->radius) || aypos < (-1 * size_y + tmp->radius)) {
		tmp->speed *= vec2(1.0f, -1.0f);
		return;
	}
	for (uint i = 0; i < Num_c; i++) {
		if (i == n)continue;
		bxpos = circles[i].ipos.x + circles[i].speed.x * base;
		bypos = circles[i].ipos.y + circles[i].speed.y * base;
		if (pow(tmp->radius + circles[i].radius, 2) > pow(axpos - bxpos, 2) + pow(aypos - bypos, 2)) {
			vec2 aspeed, bspeed;
			aspeed = tmp->speed - ((2 * circles[i].mass) / (tmp->mass + circles[i].mass)) * (dot((tmp->speed - circles[i].speed), (tmp->ipos - circles[i].ipos)) / (pow((tmp->ipos - circles[i].ipos).x, 2) + pow((tmp->ipos - circles[i].ipos).y, 2))*(tmp->ipos - circles[i].ipos));
			bspeed = circles[i].speed - ((2 * tmp->mass) / (tmp->mass + circles[i].mass)) * (dot((circles[i].speed - tmp->speed), (circles[i].ipos - tmp->ipos)) / (pow((circles[i].ipos - tmp->ipos).x, 2) + pow((circles[i].ipos - tmp->ipos).y, 2))*(circles[i].ipos - tmp->ipos));
			tmp->speed = aspeed;
			circles[i].speed = bspeed;
			return;
		}
	}
	return;
}
void render()
{
	if (base == -1)return;
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
	circle tmp;
	for (uint i = 0; i < Num_c; i++) {
		tmp = circles[i];
		collision_chk(&tmp, i);
		circles[i].speed = tmp.speed;
		circles[i].ipos += circles[i].speed * base;
		uloc = glGetUniformLocation(program, "pos");				if (uloc > -1) glUniform2fv(uloc, 1, circles[i].ipos);
		uloc = glGetUniformLocation(program, "radius");				if (uloc > -1) glUniform1f(uloc, circles[i].radius);
		uloc = glGetUniformLocation(program, "color");				if (uloc > -1) glUniform4fv(uloc, 1, circles[i].color);
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
	printf("- press 'm' to toggle between Circles-maintain_Mode and Circles-reset_Mode when the Number of circles decrease\n");
	printf("- press 'r' to reset circles attributes\n");
	printf("- press 'Numpad+' to increase the number of circles\n");
	printf("- press 'Numpad-' to decrease the number of circles\n");
	printf("		(1 <= Number of circles <= 1000)\n");
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
		else if (key == GLFW_KEY_KP_ADD) {
			if (Num_c == Num_cmax) {
				printf("Number of circles is Max\n");
			}
			else {
				printf("Number of circles : %d\n", ++Num_c);
				cir_init();
			}
		}
		else if (key == GLFW_KEY_KP_SUBTRACT) {
			if (Num_c == Num_cmin) {
				printf("Number of circles is Min\n");
			}
			else {
				printf("Number of circles : %d\n", --Num_c);
				if(brestart == true) cir_init();
				else circles.pop_back();
			}
		}
		else if (key == GLFW_KEY_M) {
			brestart = !brestart;
			printf("Circles %s when the Number of circles decrease.\n", brestart ? "have new attribute" : "maintain their attribute");
		}
		else if (key == GLFW_KEY_R) {
			printf("Reset attributes success\n");
			cir_init();
		}
	}
}

void mouse(GLFWwindow* window, int button, int action, int mods)
{
}

void motion(GLFWwindow* window, double x, double y)
{
}

void Set_vertices(uint n)
{
	vertices.clear();
	vertices.push_back({ vec3(0.0f,0.0f,0.0f),vec3(0.0f,0.0f,-1.0f),vec2(0.0f) });
	for (uint i = 0; i <= n; i++) {
		float theta = PI * 2.0f / (float)n * (float)i;
		vertices.push_back({ vec3(cos(theta), sin(theta), 0.0f), vec3(0.0f,0.0f,-1.0f), vec2(0.0f) });
	}
}

void Set_Buffer(uint n)
{
	if (vertex_buffer)	glDeleteBuffers(1, &vertex_buffer);	vertex_buffer = 0;
	if (index_buffer)	glDeleteBuffers(1, &index_buffer);	index_buffer = 0;
	if (vertices.empty()) { printf("vertices Array is empty.\n"); return; }
	indices.clear();
	for (uint i = 0; i < n; i++) {
		indices.push_back(0);
		indices.push_back(i + 1);
		indices.push_back(i + 2);
	}
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex)*vertices.size(), &vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint)*indices.size(), &indices[0], GL_STATIC_DRAW);
}

bool chk_pos(float xpos, float ypos, uint n, float r)
{
	if (xpos > (size_x - r) || xpos < (-1 * size_x + r) || ypos >(size_y - r) || ypos < (-1 * size_y + r))return false;
	for (uint i = 0; i < n; i++) {
		if (pow(r + circles[i].radius, 2) >= pow(xpos - circles[i].ipos.x, 2) + pow(ypos - circles[i].ipos.y, 2))return false;
	}
	return true;
}

void cir_init()
{
	circles.clear();
	float xpos, ypos;
	circle tmp;
	uint maxr = ((uint)(10000 * sqrt(size_x * size_y) / sqrt(Num_c * PI)));
	uint minr = uint(maxr * 0.3);
	for (uint i = 0; i < Num_c; i++) {
		tmp.radius = (rand() % (maxr - minr) + minr) / 10000.0f;
		xpos = ((rand() % (int)(10000 * size_x)) - int(5000 * size_x)) / 10000.0f;
		ypos = ((rand() % (int)(10000 * size_y)) - int(5000 * size_y)) / 10000.0f;
		tmp.speed = vec2(xpos, ypos);
		tmp.mass = tmp.radius * tmp.radius * PI;
		tmp.color = vec4((rand() % 256) / 255.0f, (rand() % 256) / 255.0f, (rand() % 256) / 255.0f, 1);
		while (1) {
			xpos = (((rand()*rand()) % int(20000 * size_x)) - int(10000 * size_x)) / 10000.0f;
			ypos = (((rand()*rand()) % int(20000 * size_y)) - int(10000 * size_y)) / 10000.0f;
			if (chk_pos(xpos, ypos, i, tmp.radius)) {
				tmp.ipos = vec2(xpos, ypos);
				break;
			}
		}
		circles.push_back(tmp);
	}
}
bool user_init()
{
	print_help();
	ratio = window_size.x / (float)window_size.y;
	ratioi = ratio;
	size_x = ratioi > 1 ? ratioi : 1;
	size_y = ratioi > 1 ? 1 : 1 / ratioi;
	glClearColor(39 / 255.0f, 40 / 255.0f, 34 / 255.0f, 1.0f);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	Set_vertices(Num_t);
	Set_Buffer(Num_t);

	srand((uint)time(NULL));
	cir_init();
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