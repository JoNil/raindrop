#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

#include "common/Shader.h"

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

void setupViewport(GLFWwindow *window, GLfloat *P);

void check_gl_error(const char * stmt, const char * fname, int line)
{
    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        printf("OpenGL error %08x, at %s:%i - for %s\n", err, fname, line, stmt);
        //abort();
    }
}

#define GL(stmt) do { \
    stmt; \
    check_gl_error(#stmt, __FILE__, __LINE__); \
} while (0)

struct Particle {
	vec2 pos;
	vec2 speed;
	vec2 acc;
	float size = 0.1;
};

//ett spår är som en tunn linje mellan olika droppar. Så man kan ha dropppunkter med olika storlekar som definierar spåret. man uppdaterar spåren genom att lägga till, ta bort och ändra storlek på droppunkterna.

Particle player;

void updateParticles(Particle particle, GLfloat *vertices, GLfloat *texs, int offset) {
	vertices[offset*18 + 0] = -particle.size / 2 + particle.pos.x;
	vertices[offset*18 + 1] = -particle.size / 2 + particle.pos.y; 
	vertices[offset*18 + 2] = 0.0f;

	vertices[offset*18 + 3] = particle.size / 2 + particle.pos.x;
	vertices[offset*18 + 4] = -particle.size / 2 + particle.pos.y;
	vertices[offset*18 + 5] = 0.0f;

    vertices[offset*18 + 6] = particle.size / 2 + particle.pos.x;
    vertices[offset*18 + 7] = particle.size / 2 + particle.pos.y;
    vertices[offset*18 + 8] = 0.0f;

    
    vertices[offset*18 + 9] = -particle.size / 2 + particle.pos.x;
    vertices[offset*18 + 10] = -particle.size / 2 + particle.pos.y; 
    vertices[offset*18 + 11] = 0.0f;

	vertices[offset*18 + 12] = particle.size / 2 + particle.pos.x;
	vertices[offset*18 + 13] = particle.size / 2 + particle.pos.y;
	vertices[offset*18 + 14] = 0.0f;

	vertices[offset*18 + 15] = -particle.size / 2 + particle.pos.x;
	vertices[offset*18 + 16] = particle.size / 2 + particle.pos.y;
	vertices[offset*18 + 17] = 0.0f;


    texs[offset*12 + 0] = 0.0f;
    texs[offset*12 + 1] = 0.0f;

    texs[offset*12 + 2] = 1.0f;
    texs[offset*12 + 3] = 0.0f;

    texs[offset*12 + 4] = 1.0f;
    texs[offset*12 + 5] = 1.0f;


    texs[offset*12 + 6] = 0.0f;
    texs[offset*12 + 7] = 0.0f;

    texs[offset*12 + 8] = 1.0f;
    texs[offset*12 + 9] = 1.0f;

    texs[offset*12 + 10] = 0.0f;
    texs[offset*12 + 11] = 1.0f;
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key == GLFW_KEY_UP && action == GLFW_PRESS){
		std::cout << "up\n";
		player.pos.y += 0.1;
		//position += direction * deltaTime * speed;
	}
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS){
		player.pos.y += -0.1;
		//position -= direction * deltaTime * speed;
	}
	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS){
		player.pos.x += 0.1;
		//position += right * deltaTime * speed;
	}
	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS){
		player.pos.x += -0.1;
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
    GLfloat P[16] = { 2.42f, 0.0f, 0.0f, 0.0f,
                    0.0f, 2.42f, 0.0f, 0.0f,
                    0.0f, 0.0f, -1.0f, -1.0f,
                    0.0f, 0.0f, -0.2f, 0.0f };

    int width = 720;
    int height = 720;

    std::srand(std::time(0));

   //GL INITIALIZATION \___________________________________________
    // start GLEW extension handler
    if (!glfwInit()) {
            fprintf(stderr, "ERROR: could not start GLFW3\n");
            return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR , 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //create GLFW window and select context
    GLFWwindow* window = glfwCreateWindow(640, 480, "TNM084", NULL, NULL);
    if (!window) {
        fprintf(stderr, "ERROR: could not open window with GLFW3\n");
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);

    //start GLEW extension handler
    glewExperimental = GL_TRUE;
    glewInit();

    // get version info
    const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
    const GLubyte* version = glGetString(GL_VERSION); // version as a string
    printf("Renderer: %s\n", renderer);
    printf("OpenGL version supported %s\n", version);

    //initialize particle system
    const int numParticles = 50;

    std::vector<Particle> particles(numParticles);
	
    for (int i = 0; i < (int)particles.size(); ++i) {
		particles[i].pos.x = (float)(std::rand() % 200) / 100.0f - 1;
		particles[i].pos.y = (float)(std::rand() % 200) / 100.0f - 1;
    }

	//init player particle
	player.pos.x = 0;
	player.pos.y = 0;
	player.size = 0.5;

	//GLuint vertexbuffer_player;
	//glGenBuffer

   setupViewport(window, P);
    GL(glClearColor(0.0f, 0.0f, 0.2f, 1.0f));
    GL(glDisable(GL_DEPTH_TEST));
    GL(glEnable(GL_BLEND));
    GL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    int status = 0;
    uint32_t background_tex = load_background("background.jpg");

    float oldTime = 0;
    float newTime = glfwGetTime();

    GLuint VAO;
    GLuint POS_VBO;
    GLuint TEX_VBO;
    std::vector<GLfloat> particleVertices((numParticles + 1) * 18);
    std::vector<GLfloat> particleTexs((numParticles + 1) * 12);
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
    
    Shader raindropShader;
    raindropShader.createShader("shader/raindrop.vert", "shader/raindrop.frag", "shader/raindrop.geom");
    Shader background_shader("shader/background.vert", "shader/background.frag");

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

		updateParticles(player, particleVertices.data(), particleTexs.data(), (int)particles.size());

        GL(glUseProgram(background_shader.programID));
        GL(glBindTexture(GL_TEXTURE_2D, background_tex));
        GL(glActiveTexture(GL_TEXTURE0));
        GL(glUniform1i(glGetUniformLocation(background_shader.programID, "tex"), 0));
        GL(glBindVertexArray(QUAD_VAO));
        GL(glDrawArrays(GL_TRIANGLES, 0, 6));

        GL(glBindBuffer(GL_ARRAY_BUFFER, POS_VBO));
		GL(glBufferData(GL_ARRAY_BUFFER, particleVertices.size() * 4, particleVertices.data(), GL_STATIC_DRAW));
        GL(glBindBuffer(GL_ARRAY_BUFFER, TEX_VBO));
        GL(glBufferData(GL_ARRAY_BUFFER, particleTexs.size() * 4, particleTexs.data(), GL_STATIC_DRAW));

        GL(glUseProgram(shader.programID));
        GL(glBindVertexArray(VAO));
		GL(glDrawArrays(GL_TRIANGLES, 0, (numParticles+1)*4));

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void setupViewport(GLFWwindow *window, GLfloat *P) {
	int width, height;

	glfwGetWindowSize(window, &width, &height);

	P[0] = P[5] * height / width;

	glViewport(0, 0, width, height);
}
