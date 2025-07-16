#include <cglm/vec2.h>
#include <stdio.h>
#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <cglm/cglm.h>

#include <miniaudio/miniaudio.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

int main(void) {
	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(640, 480, "Window", NULL, NULL);
	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	gladLoadGL(glfwGetProcAddress);

	ma_result result;
	ma_engine engine;

	result = ma_engine_init(NULL, &engine);
	if (result != MA_SUCCESS) {
		return -1;
	}

	bool space_key_state = false;

	vec2 mult = { 3, 4 };
	vec2 vector = { 1, 2 };
	glm_vec2_mul(mult, vector, vector);

	printf("Vector { %.2f, %.2f }\n", vector[0], vector[1]);

	while (!glfwWindowShouldClose(window)) {
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		if (space_key_state == GLFW_RELEASE && space_key_state != glfwGetKey(window, GLFW_KEY_SPACE))
			ma_engine_play_sound(&engine, "./assets/sound/coin.wav", NULL);

		space_key_state = glfwGetKey(window, GLFW_KEY_SPACE);

		glViewport(0, 0, width, height);
		glClearColor(255, 255, 255, 255);
		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);

	ma_engine_uninit(&engine);

	glfwTerminate();
	exit(EXIT_SUCCESS);
}
