#include "renderer.h"
#include "shader.h"

#include "core/arena.h"
#include "core/logger.h"

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <cglm/cglm.h>

#include <miniaudio/miniaudio.h>
#include <stb/stb_image.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct _state {
	Arena *permanent;
} State;


void gl_message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const *message, void const *user_param);

int main(void) {
	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	GLFWwindow *window = glfwCreateWindow(640, 480, "Window", NULL, NULL);
	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	gladLoadGL(glfwGetProcAddress);

	State state = {
		.permanent = arena_alloc()
	};

	renderer_create(state.permanent);
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_BLEND); // you enable blending function
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDebugMessageCallback(gl_message_callback, NULL);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);

	Shader *shader = shader_create_file(state.permanent, "./assets/shaders/v_default.glsl", "./assets/shaders/f_default.glsl");

	// clang-format off
	float vertices[] = {
		-0.5f,  0.5f, 0.0f, 1.0f,   // top left
		 0.5f,  0.5f, 1.0f, 1.0f,   // top right
		-0.5f, -0.5f, 0.0f, 0.0f,   // bottom left
		 0.5f, -0.5f, 1.0f, 0.0f    // bottom right
	};
	unsigned int indices[] = {
		// note that we start from 0!
		0, 2, 1, // first triangle
		1, 2, 3 // second triangle
	};
	// clang-format on

	uint32_t vao, vbo, ibo;

	glCreateBuffers(1, &vbo);
	glNamedBufferStorage(vbo, sizeof(vertices), vertices, GL_DYNAMIC_STORAGE_BIT);

	glCreateBuffers(1, &ibo);
	glNamedBufferStorage(ibo, sizeof(indices), indices, GL_DYNAMIC_STORAGE_BIT);

	glCreateVertexArrays(1, &vao);

	glVertexArrayVertexBuffer(vao, 0, vbo, 0, 4 * sizeof(*vertices));
	glVertexArrayElementBuffer(vao, ibo);

	glEnableVertexArrayAttrib(vao, 0);
	glEnableVertexArrayAttrib(vao, 1);

	glVertexArrayAttribFormat(vao, 0, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribFormat(vao, 1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(*vertices));
	glVertexArrayAttribBinding(vao, 0, 0);
	glVertexArrayAttribBinding(vao, 1, 0);

	stbi_set_flip_vertically_on_load(true);

	uint32_t texture;
	glCreateTextures(GL_TEXTURE_2D, 1, &texture);
	// set the texture wrapping parameters
	glTextureParameteri(texture, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTextureParameteri(texture, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// load image, create texture and generate mipmaps
	int32_t width, height, channels;
	char path[] = "./assets/sprites/tile_0085.png";
	uint8_t *data = stbi_load(path, &width, &height, &channels, 0);
	if (data) {
		glTextureStorage2D(texture, 1, GL_RGBA8, width, height);
		glTextureSubImage2D(texture, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
	} else
		LOG_ERROR("PATH: %s not found");

	stbi_image_free(data);
	while (!glfwWindowShouldClose(window)) {
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		glViewport(0, 0, width, height);
		glClearColor(255, 255, 255, 255);
		glClear(GL_COLOR_BUFFER_BIT);

		shader_activate(shader);

		glBindTextureUnit(0, texture);
		shader_seti(shader, "u_texture", 0);

		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);

	glfwTerminate();
	exit(EXIT_SUCCESS);
}

void gl_message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei size, GLchar const *message, void const *user_param) {
	char src_str[32];

	switch (source) {
		case GL_DEBUG_SOURCE_API: {
			snprintf(src_str, sizeof(src_str), "API");
		} break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM: {
			snprintf(src_str, sizeof(src_str), "WINDOW SYSTEM");
		} break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER: {
			snprintf(src_str, sizeof(src_str), "SHADER COMPILER");
		} break;
		case GL_DEBUG_SOURCE_THIRD_PARTY: {
			snprintf(src_str, sizeof(src_str), "THIRD PARTY");
		} break;
		case GL_DEBUG_SOURCE_APPLICATION: {
			snprintf(src_str, sizeof(src_str), "APPLICATION");
		} break;
		case GL_DEBUG_SOURCE_OTHER: {
			snprintf(src_str, sizeof(src_str), "OTHER");
		} break;
	}

	char type_str[32];

	switch (type) {
		case GL_DEBUG_TYPE_ERROR: {
			snprintf(type_str, sizeof(type_str), "ERROR");
		} break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: {
			snprintf(type_str, sizeof(type_str), "DEPRECATED_BEHAVIOR");
		} break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: {
			snprintf(type_str, sizeof(type_str), "UNDEFINED_BEHAVIOR");
		} break;
		case GL_DEBUG_TYPE_PORTABILITY: {
			snprintf(type_str, sizeof(type_str), "PORTABILITY");
		} break;
		case GL_DEBUG_TYPE_PERFORMANCE: {
			snprintf(type_str, sizeof(type_str), "PERFORMANCE");
		} break;
		case GL_DEBUG_TYPE_MARKER: {
			snprintf(type_str, sizeof(type_str), "MARKER");
		} break;
		case GL_DEBUG_TYPE_OTHER: {
			snprintf(type_str, sizeof(type_str), "OTHER");
		} break;
	}

	char severity_str[32];

	switch (severity) {
		case GL_DEBUG_SEVERITY_NOTIFICATION: {
			snprintf(severity_str, sizeof(severity_str), "NOTIFICATION");
		} break;
		case GL_DEBUG_SEVERITY_LOW: {
			snprintf(severity_str, sizeof(severity_str), "LOW");
		} break;
		case GL_DEBUG_SEVERITY_MEDIUM: {
			snprintf(severity_str, sizeof(severity_str), "MEDIUM");
		} break;
		case GL_DEBUG_SEVERITY_HIGH: {
			snprintf(severity_str, sizeof(severity_str), "HIGH");
		} break;
	}

	printf("%s, %s, %s, %d: %s\n", src_str, type_str, severity_str, id, message);
}
