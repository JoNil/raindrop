#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cstdint>
#include <vector>
#include <string>

using namespace glm;

void check_gl_error(const char * stmt, const char * fname, int line)
{
    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        printf("OpenGL error %08x, at %s:%i - for %s\n", err, fname, line, stmt);
        abort();
    }
}

#define GL(stmt) do { \
    stmt; \
    check_gl_error(#stmt, __FILE__, __LINE__); \
} while (0)

#define GLSL(version, shader) "#version " #version "\n" #shader

const char * vertex_shader_source = GLSL(130,

attribute vec3 vertex_pos;
attribute vec2 vertex_tex;

varying vec3 pos;
varying vec2 tex;

void main() {
    pos = vertex_pos;
    tex = vertex_tex;
    gl_Position = vec4(vertex_pos, 1.0);
});

//const char * geometry_shader_source = GLSL(330,
//	
//
//	varying vec3 pos;
//
//void main()
//{
//	gl_Position = vec4(position.x, position.y, 0.0f, 1.0f);
//});

const char * fragment_shader_source = GLSL(130,

varying vec3 pos;
varying vec2 tex;

void main()
{
    vec3 color = vec3(191.0/255.0, 0.0, 1.0);

    vec2 local = (tex - 0.5) * 2.0;

    float r = sqrt(local.x*local.x + local.y*local.y);
	r = clamp(r, 0.0, 1.0);
	float alpha = 1.5;
	alpha*= pow(1.0 - r, 2.0);

    gl_FragColor = vec4(color, alpha);
});

static void print_shader_compile_error(uint32_t shader)
{
    std::string infolog;

    int length = 0;
    GL(glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length));

    infolog.resize((size_t)length);

    GL(glGetShaderInfoLog(shader, length, nullptr, &infolog[0]));

    printf("%s\n", infolog.c_str());
}

struct Particle {
	vec2 pos;
	vec2 speed;
	vec2 acc;
	float size = 0.1;
};

//ett spår är som en tunn linje mellan olika droppar. Så man kan ha dropppunkter med olika storlekar som definierar spåret. man uppdaterar spåren genom att lägga till, ta bort och ändra storlek på droppunkterna.

void updateParticles(Particle particle, GLfloat *vertices, GLfloat *texs, int offset) {
	vertices[offset*12 + 0] = -particle.size / 2 + particle.pos.x;
	vertices[offset*12 + 1] = -particle.size / 2 + particle.pos.y; 
	vertices[offset*12 + 2] = 0.0f;

	vertices[offset*12 + 3] = particle.size / 2 + particle.pos.x;
	vertices[offset*12 + 4] = -particle.size / 2 + particle.pos.y;
	vertices[offset*12 + 5] = 0.0f;

	vertices[offset*12 + 6] = particle.size / 2 + particle.pos.x;
	vertices[offset*12 + 7] = particle.size / 2 + particle.pos.y;
	vertices[offset*12 + 8] = 0.0f;

	vertices[offset*12 + 9] = -particle.size / 2 + particle.pos.x;
	vertices[offset*12 + 10] = particle.size / 2 + particle.pos.y;
	vertices[offset*12 + 11] = 0.0f;

    texs[offset*8 + 0] = 0.0f;
    texs[offset*8 + 1] = 0.0f;

    texs[offset*8 + 2] = 1.0f;
    texs[offset*8 + 3] = 0.0f;

    texs[offset*8 + 4] = 1.0f;
    texs[offset*8 + 5] = 1.0f;

    texs[offset*8 + 6] = 0.0f;
    texs[offset*8 + 7] = 1.0f;
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key == GLFW_KEY_UP && action == GLFW_PRESS){
		std::cout << "up\n";
		//position += direction * deltaTime * speed;
	}
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS){
		//position -= direction * deltaTime * speed;
	}
	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS){
		//position += right * deltaTime * speed;
	}
	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS){
		//position -= right * deltaTime * speed;
	}
}

uint32_t load_background(const char * path)
{
    uint32_t tex_id = 0;

    int32_t width = 0;
    int32_t height = 0;
    int32_t comp = 0;
    uint8_t * image = stbi_load(path, &width, &height, &comp, STBI_rgb);

    if (image == nullptr) {
        exit(1);
    }

    glGenTextures(1, &tex_id);

    glBindTexture(GL_TEXTURE_2D, tex_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (comp == 3) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    } else if(comp == 4) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    }

    return tex_id;
}

void simulate_particles(Particle * particles, int particle_count, float dt)
{
	int type;
    for (int i = 0; i < particle_count; ++i) {
		if (particles[i].pos.y < -1)
			particles[i].pos.y = 1;
		if (particles[i].pos.x < -1)
			particles[i].pos.x = 1;

		//RAND_MAX = 32767
		type = std::rand() % 100;
		if (type < 1)
		{
			particles[i].acc.y = -(float)(std::rand() % 100) / 2.0f;
			particles[i].acc.x = -(float)(std::rand() % 100) / 2.0f;
		}

		if (i < particle_count/4)
		{
			particles[i].speed.y = -(float)(std::rand() % 10) / 15.0f;
			particles[i].speed.x = -(float)(std::rand() % 10) / 15.0f;
		}
		else if (i < particle_count/2)
		{
			particles[i].speed.y = -(float)(std::rand() % 10) / 100.0f;
			particles[i].speed.x = -(float)(std::rand() % 10) / 100.0f;
		}
		else
		{
			particles[i].speed.y = 0;
			particles[i].speed.x = 0;
		}

		particles[i].speed += particles[i].acc * dt;
		particles[i].pos += particles[i].speed * dt;
	}
}

static float quad_vertices[] = {
    -1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,

    -1.0f, 1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    -1.0f, -1.0f, 0.0f,
};

static float quad_uvs[] = {
    0.0f, 1.0f,
    1.0f, 1.0f,
    1.0f, 0.0f,

    0.0f, 1.0f,
    1.0f, 0.0f,
    0.0f, 0.0f,
};

int main(int argc, char ** argv)
{
    int width = 720;
    int height = 720;

    std::srand(std::time(0));

    glfwInit();

    GLFWwindow * window = glfwCreateWindow(width, height, "Friday particle", NULL, NULL);
	glfwSwapInterval(1);

    glfwSetKeyCallback(window, key_callback);

    glfwMakeContextCurrent(window);
	
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	const int numParticles = 50;
    std::vector<Particle> particles(numParticles);
	
    for (int i = 0; i < (int)particles.size(); ++i) {
		particles[i].pos.x = (float)(std::rand() % 200) / 100.0f - 1;
		particles[i].pos.y = (float)(std::rand() % 200) / 100.0f - 1;
    }

    GL(glViewport(0, 0, width, height));
    GL(glClearColor(0.0f, 0.0f, 0.2f, 1.0f));
    GL(glDisable(GL_DEPTH_TEST));
	GL(glEnable(GL_BLEND));
	GL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    int status = 0;
    uint32_t vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	//uint32_t geometry_shader = glCreateShader(GL_GEOMETRY_SHADER);
    uint32_t fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    GL(glShaderSource(vertex_shader, 1, &vertex_shader_source, nullptr));
    GL(glShaderSource(fragment_shader, 1, &fragment_shader_source, nullptr));

    GL(glCompileShader(vertex_shader));
    GL(glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &status));
    if (status == GL_FALSE) {
        printf("Vertex shader error:\n");
        print_shader_compile_error(vertex_shader);
        exit(1);
    }

    GL(glCompileShader(fragment_shader));
    GL(glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &status));
    if (status == GL_FALSE) {
        printf("Fragment shader error:\n");
        print_shader_compile_error(fragment_shader);
        exit(1);
    }

    uint32_t shader_program = glCreateProgram();
    GL(glAttachShader(shader_program, vertex_shader));
    GL(glAttachShader(shader_program, fragment_shader));
    GL(glLinkProgram(shader_program));

    GL(glValidateProgram(shader_program));
    GL(glGetProgramiv(shader_program, GL_VALIDATE_STATUS, &status));
    if (status == GL_FALSE) {
        printf("Link error\n");
        print_shader_compile_error(shader_program);
        exit(1);
    }

    GL(glUseProgram(shader_program));

    load_background("background.jpg");

	float oldTime = 0;
	float newTime = glfwGetTime();

    GLuint VAO;
	GLuint POS_VBO;
    GLuint TEX_VBO;
	std::vector<GLfloat> particleVertices(numParticles * 12);
    std::vector<GLfloat> particleTexs(numParticles * 8);
    GL(glGenVertexArrays(1, &VAO));
    GL(glBindVertexArray(VAO));
	GL(glGenBuffers(1, &POS_VBO));
    GL(glGenBuffers(1, &TEX_VBO));
    GL(glBindBuffer(GL_ARRAY_BUFFER, POS_VBO));
	GL(glEnableVertexAttribArray(0));
	GL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0));
    GL(glBindBuffer(GL_ARRAY_BUFFER, TEX_VBO));
    GL(glEnableVertexAttribArray(1));
    GL(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0));

    GLuint QUAD_VAO;
    GLuint QUAD_VERT_VBO;
    GLuint QUAD_UV_VBO;
    GL(glGenVertexArrays(1, &QUAD_VAO));
    GL(glBindVertexArray(QUAD_VAO));
    GL(glGenBuffers(1, &QUAD_VERT_VBO));
    GL(glGenBuffers(1, &QUAD_UV_VBO));
    GL(glBindBuffer(GL_ARRAY_BUFFER, QUAD_VERT_VBO));
    GL(glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices) * 4, quad_vertices, GL_STATIC_DRAW));
    GL(glEnableVertexAttribArray(0));
    GL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0));
    GL(glBindBuffer(GL_ARRAY_BUFFER, QUAD_UV_VBO));
    GL(glBufferData(GL_ARRAY_BUFFER, sizeof(quad_uvs) * 4, quad_uvs, GL_STATIC_DRAW));
    GL(glEnableVertexAttribArray(1));
    GL(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0));


    while (!glfwWindowShouldClose(window)) {

		oldTime = newTime;
		newTime = glfwGetTime();
		float deltaTime = newTime - oldTime;

        GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        simulate_particles(particles.data(), particles.size(), deltaTime);

        for (int i = 0; i < (int)particles.size(); ++i) {
            updateParticles(particles[i], particleVertices.data(), particleTexs.data(), i);	
        }

        GL(glBindVertexArray(QUAD_VAO));
        GL(glDrawArrays(GL_TRIANGLES, 0, 6));

        GL(glBindBuffer(GL_ARRAY_BUFFER, POS_VBO));
		GL(glBufferData(GL_ARRAY_BUFFER, particleVertices.size() * 4, particleVertices.data(), GL_STATIC_DRAW));
        GL(glBindBuffer(GL_ARRAY_BUFFER, TEX_VBO));
        GL(glBufferData(GL_ARRAY_BUFFER, particleTexs.size() * 4, particleTexs.data(), GL_STATIC_DRAW));

        GL(glBindVertexArray(VAO));
		GL(glDrawArrays(GL_QUADS, 0, numParticles*4));

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}
