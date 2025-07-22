#pragma once

#include <stdint.h>

typedef struct _arena Arena;
typedef struct _shader Shader;

Shader *shader_create_file(Arena *arena, const char *vertex_shader_path, const char *fragment_shader_path);
Shader *shader_create(Arena *arena, const char *vertex_shader_source, const char *fragment_shader_source);

void shader_activate(const Shader *shader);
void shader_deactivate(const Shader *shader);

void shader_seti(Shader *shader, const char *name, int32_t value);
void shader_setf(Shader *shader, const char *name, float value);
void shader_set2fv(Shader *shader, const char *name, float *value);
void shader_set3fv(Shader *shader, const char *name, float *value);
void shader_set4fv(Shader *shader, const char *name, float *value);
void shader_set4fm(Shader *shader, const char *name, float *value);
