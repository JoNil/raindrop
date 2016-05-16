#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

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

struct Particle {
	vec2 pos;
	vec2 speed;
	vec2 acc;
	float size;
};

//ett spår är som en tunn linje mellan olika droppar. Så man kan ha dropppunkter med olika storlekar som definierar spåret. man uppdaterar spåren genom att lägga till, ta bort och ändra storlek på droppunkterna.

Particle player;
glm::vec2 wind(-1, -1);
bool left_down = false;
bool right_down = false;

void updateParticles(Particle particle, GLfloat *vertices, GLfloat *sizes, int offset) {
	vertices[offset*2 + 0] = particle.pos.x;
	vertices[offset*2 + 1] = particle.pos.y; 

	sizes[offset] = particle.size;
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

void particle_collision_response(Particle * a, Particle * b)
{
    float old_size = a->size;

    a->size = sqrt(a->size * a->size + b->size * b->size);
    a->pos = (old_size * a->pos + b->size * b->pos) / (old_size + b->size);
    a->speed = (old_size * a->speed + b->size * b->speed) / (old_size + b->size);

    b->pos = vec2(0.0f, 0.0f);
    b->speed = vec2(0.0f, 0.0f);
    b->size = 0.0f;
}

void physics_step(Particle * particle, vec2 external_force, float dt)
{
    particle->acc = (external_force / (50.0f * sqrtf(particle->size)));
    particle->acc -= particle->speed;
    particle->speed += particle->acc * dt;
    particle->pos += particle->speed * dt;
}

void simulate_particles(std::vector<Particle> * particlesIn, float dt)
{
	Particle * particles = particlesIn->data();
	int particle_count = particlesIn->size();

    for (int i = 0; i < particle_count; ++i) {
		
        //keep in bounderies around the player
		if (particles[i].pos.y < player.pos.y - 2) {
			particles[i].size = 0;
        }
		else if (particles[i].pos.x < player.pos.x - 2) {
			particles[i].size = 0;
        }

        //respawn dead
		if (particles[i].size == 0) {
			//this particle was absorbed or reached the boundary
			//and should be reinitialized

            //        ______
            //       /      |
            //    /  |   1  |
            // /___2_|______|
            // |     | 3   /
            // |  G  |   /
            // |_____|/
			if ((float)(std::rand() % 2) >= 1){
				particles[i].pos.x = (float)(std::rand() % 200) / 100.0f + 1;
				particles[i].pos.y = (float)(std::rand() % 200) / 100.0f + 1;
			}
			else
			{
				particles[i].pos.x = (float)(std::rand() % 200) / 100.0f - 1;
				particles[i].pos.y = (float)(std::rand() % 200) / 100.0f - 1;

				if (particles[i].pos.y > particles[i].pos.x) {
					particles[i].pos.x += 2.0f;
				}
				else {
					particles[i].pos.y += 2.0f;
				}
			}
			particles[i].size = (float)(std::rand() % 60) / 1000.0f + 0.03;
		}

        //simulation
		physics_step(&particles[i], wind, dt);

		//collision
		for (int j = i + 1; j < particle_count; j++)
		{
			float dist = glm::length(particles[j].pos - particles[i].pos);
			//std::cout << "dist " << dist << "\n";
			if (dist < particles[j].size + particles[i].size){
				//std::cout << "collision\n";
				//collision detected
				if (particles[j].size <= particles[i].size)
				{
					//i absorbes j
					//calculate new radius for new area
					particle_collision_response(&particles[i], &particles[j]);
				}
				else{
					//j absorbes i
					//calculate new radius for new area
					particle_collision_response(&particles[j], &particles[i]);
				}
			}

		}
	}
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

	physics_step(&player, acc, dt);

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

    std::vector<Particle> particles(numParticles);
	
    for (int i = 0; i < (int)particles.size(); ++i) {
		particles[i].pos.x = (float)(std::rand() % 200) / 100.0f - 1;
		particles[i].pos.y = (float)(std::rand() % 200) / 100.0f - 1;
		particles[i].size = (float)(std::rand() % 60) / 1000.0f + 0.03;
    }

    //init player particle
    player.pos.x = 0;
    player.pos.y = 0;
    player.acc.x = 0;
    player.acc.y = 0;
    player.size = 0.05;
    player.speed = vec2(0.0f, 0.0f);

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
    std::vector<GLfloat> particleVertices((numParticles + 1) * 2);
    std::vector<GLfloat> particleSize((numParticles + 1) * 1);
    GL(glGenVertexArrays(1, &VAO));
    GL(glBindVertexArray(VAO));
    GL(glGenBuffers(1, &POS_VBO));
    GL(glGenBuffers(1, &SIZE_VBO));
    GL(glBindBuffer(GL_ARRAY_BUFFER, POS_VBO));
    GL(glEnableVertexAttribArray(0));
    GL(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0));
    GL(glBindBuffer(GL_ARRAY_BUFFER, SIZE_VBO));
    GL(glEnableVertexAttribArray(1));
    GL(glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 1 * sizeof(GLfloat), (GLvoid*)0));
    
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

        simulate_particles(&particles, deltaTime);
        simulate_player(deltaTime);

        for (int i = 0; i < (int)particles.size(); ++i) {
            updateParticles(particles[i], particleVertices.data(), particleSize.data(), i);	
        }

        updateParticles(player, particleVertices.data(), particleSize.data(), (int)particles.size());

        glm::mat4 mvp = glm::mat4(1.0f);
        mvp[3][0] = -player.pos.x;
        mvp[3][1] = -player.pos.y;

        GL(glUseProgram(background_shader.programID));
        GL(glBindTexture(GL_TEXTURE_2D, background_tex));
        GL(glActiveTexture(GL_TEXTURE0));
        GL(glUniform2f(glGetUniformLocation(background_shader.programID, "offset"), player.pos.x, player.pos.y));
        GL(glUniform1i(glGetUniformLocation(background_shader.programID, "tex"), 0));
        GL(glBindVertexArray(QUAD_VAO));
        GL(glDrawArrays(GL_TRIANGLES, 0, 6));

        GL(glBindBuffer(GL_ARRAY_BUFFER, POS_VBO));
        GL(glBufferData(GL_ARRAY_BUFFER, particleVertices.size() * 4, particleVertices.data(), GL_STATIC_DRAW));
        GL(glBindBuffer(GL_ARRAY_BUFFER, SIZE_VBO));
        GL(glBufferData(GL_ARRAY_BUFFER, particleSize.size() * 4, particleSize.data(), GL_STATIC_DRAW));

        GL(glUseProgram(raindropShader.programID));

        glUniformMatrix4fv(MVPID, 1, GL_FALSE, &mvp[0][0]);

        GL(glBindVertexArray(VAO));
        GL(glDrawArrays(GL_POINTS, 0, numParticles + 1));

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}
