#include "renderer.h"

#include "core/arena.h"

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

typedef struct _state {
	Arena *permanent;
} State;

static const char *vertex_shader_source = "#version 450 core\n"
										  "layout (location = 0) in vec2 a_position;\n"
										  "layout (location = 1) in vec2 a_texcoord;\n"
										  "void main()\n"
										  "{\n"
										  "   gl_Position = vec4(a_position, 0.0f, 1.0);\n"
										  "}\0";
static const char *fragment_shader_source = "#version 450 core\n"
											"out vec4 fragment_color;\n"
											"void main()\n"
											"{\n"
											"   fragment_color = vec4(vec3(0.3f), 1.0f);\n"
											"}\0";

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
	glDebugMessageCallback(gl_message_callback, NULL);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);

	uint32_t vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
	glCompileShader(vertex_shader);

	uint32_t fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
	glCompileShader(fragment_shader);

	uint32_t program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);

	glDetachShader(program, vertex_shader);
	glDetachShader(program, fragment_shader);
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

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
	glVertexArrayAttribFormat(vao, 1, 2, GL_FLOAT, GL_FALSE, 2);

	while (!glfwWindowShouldClose(window)) {
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		glViewport(0, 0, width, height);
		glClearColor(255, 255, 255, 255);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(program);
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
