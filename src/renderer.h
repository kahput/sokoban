#pragma once

typedef struct _camera Camera;
typedef struct _arena Arena;

void renderer_create(Arena *arena);

void renderer_begin_frame(const Camera *camera);
void renderer_end_frame();

void renderer_submit();
