#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "common/Shader.h"
#include "common/Particle.h"
#include "common/ParticleSystem.h"

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
        //printf("OpenGL error %08x, at %s:%i - for %s\n", err, fname, line, stmt);
        //abort();
    }
}

#define GL(stmt) do { \
    stmt; \
    check_gl_error(#stmt, __FILE__, __LINE__); \
} while (0)



//ett sp�r �r som en tunn linje mellan olika droppar. S� man kan ha dropppunkter med olika storlekar som definierar sp�ret. man uppdaterar sp�ren genom att l�gga till, ta bort och �ndra storlek p� droppunkterna.

Particle player;
glm::vec2 wind(-1, -1);
bool left_down = false;
bool right_down = false;

void updateParticles(Particle particle, GLfloat *vertices, GLfloat *sizes, GLfloat* vel, int offset) {
	vertices[offset*2 + 0] = particle._pos.x;
	vertices[offset*2 + 1] = particle._pos.y;

	vel[offset*2 + 0] = particle._speed.x;
	vel[offset*2 + 1] = particle._speed.y;

	sizes[offset] = particle._size;
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if ((key == GLFW_KEY_RIGHT || key == GLFW_KEY_D) && action == GLFW_PRESS){
		right_down = true;
	}
	if ((key == GLFW_KEY_RIGHT || key == GLFW_KEY_D) && action == GLFW_RELEASE){
		right_down = false;
	}
	if ((key == GLFW_KEY_LEFT || key == GLFW_KEY_A) && action == GLFW_PRESS){
		left_down = true;
	}
	if ((key == GLFW_KEY_LEFT || key == GLFW_KEY_A) && action == GLFW_RELEASE){
		left_down = false;
	}
}

void resize(GLFWwindow * window, int width, int height) {
    glViewport(0, 0, width, height);
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

void simulate_player(float dt) {

	//wind
	vec2 acc = wind;

	//input
	const float accConst = 3.0;
	if (left_down){
		acc.x -= accConst;
		acc.y += accConst;
	}
	if (right_down) {
		acc.x += accConst;
		acc.y -= accConst;
	}
	player.physics_step(acc, dt);

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
    int width = 1024;
    int height = 720;

    std::srand(std::time(0));

    if (!glfwInit()) {
        fprintf(stderr, "ERROR: could not start GLFW3\n");
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR , 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //create GLFW window and select context
    GLFWwindow* window = glfwCreateWindow(width, height, "raindrop", NULL, NULL);
    if (!window) {
        fprintf(stderr, "ERROR: could not open window with GLFW3\n");
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, resize);

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
	ParticleSystem particles(numParticles, &player, wind);

    //init player particle
    player._pos.x = 0;
    player._pos.y = 0;
    player._acc.x = 0;
    player._acc.y = 0;
    player._size = 0.05;
    player._speed = vec2(0.0f, 0.0f);

    resize(window, width, height);
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
    GLuint SIZE_VBO;
    GLuint vel_VBO;
    std::vector<GLfloat> particleVertices((numParticles + 1) * 2);
    std::vector<GLfloat> particleVel((numParticles + 1) * 2);
    std::vector<GLfloat> particleSize((numParticles + 1) * 1);

    GL(glGenVertexArrays(1, &VAO));
    GL(glBindVertexArray(VAO));
    GL(glGenBuffers(1, &POS_VBO));
    GL(glGenBuffers(1, &SIZE_VBO));
    GL(glGenBuffers(1, &vel_VBO));

    GL(glBindBuffer(GL_ARRAY_BUFFER, POS_VBO));
    GL(glEnableVertexAttribArray(0));
    GL(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0));

    GL(glBindBuffer(GL_ARRAY_BUFFER, SIZE_VBO));
    GL(glEnableVertexAttribArray(1));
    GL(glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 1 * sizeof(GLfloat), (GLvoid*)0));

    GL(glBindBuffer(GL_ARRAY_BUFFER, vel_VBO));
    GL(glEnableVertexAttribArray(2));
    GL(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0));

    Shader raindropShader;
    raindropShader.createShader("shader/raindrop.vert", "shader/raindrop.frag", "shader/raindrop.geom");
    GLuint MVPID = glGetUniformLocation(raindropShader.programID, "MVP");

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

        particles.simulate_particles(deltaTime);
        simulate_player(deltaTime);

        for (int i = 0; i < (int)particles._particles.size(); ++i) {
            updateParticles(particles._particles[i], particleVertices.data(), particleSize.data(), particleVel.data(), i);
        }

        updateParticles(player, particleVertices.data(), particleSize.data(), particleVel.data(), (int)particles._particles.size());

        glm::mat4 mvp = glm::mat4(1.0f);
        mvp[3][0] = -player._pos.x;
        mvp[3][1] = -player._pos.y;

        GL(glUseProgram(background_shader.programID));
        GL(glBindTexture(GL_TEXTURE_2D, background_tex));
        GL(glActiveTexture(GL_TEXTURE0));
        GL(glUniform2f(glGetUniformLocation(background_shader.programID, "offset"), player._pos.x, player._pos.y));
        GL(glUniform1i(glGetUniformLocation(background_shader.programID, "tex"), 0));
        GL(glBindVertexArray(QUAD_VAO));
        GL(glDrawArrays(GL_TRIANGLES, 0, 6));

        GL(glBindBuffer(GL_ARRAY_BUFFER, POS_VBO));
        GL(glBufferData(GL_ARRAY_BUFFER, particleVertices.size() * 4, particleVertices.data(), GL_DYNAMIC_DRAW));

        GL(glBindBuffer(GL_ARRAY_BUFFER, SIZE_VBO));
        GL(glBufferData(GL_ARRAY_BUFFER, particleSize.size() * 4, particleSize.data(), GL_DYNAMIC_DRAW));

        GL(glBindBuffer(GL_ARRAY_BUFFER, vel_VBO));
        GL(glBufferData(GL_ARRAY_BUFFER, particleVel.size() * 4, particleVel.data(), GL_DYNAMIC_DRAW));

        GL(glUseProgram(raindropShader.programID));

        glUniformMatrix4fv(MVPID, 1, GL_FALSE, &mvp[0][0]);

        GL(glBindVertexArray(VAO));
        GL(glDrawArrays(GL_POINTS, 0, numParticles + 1));

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}
