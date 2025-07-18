#include "renderer.h"

#include "core/arena.h"
#include "core/logger.h"

typedef struct _renderer {
	uint32_t vao;
} Renderer;

static Renderer *g_renderer;

void renderer_create(Arena *arena) {
	g_renderer = arena_push_type(arena, Renderer);

	*g_renderer = (Renderer){
		.vao = 0
	};
}

void renderer_begin_frame(const Camera *camera) {}
void renderer_end_frame() {}

void renderer_submit() {}
