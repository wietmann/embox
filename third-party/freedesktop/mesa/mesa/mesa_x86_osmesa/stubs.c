/*
 * stubs.c
 *
 *  Created on: Jul 7, 2017
 *      Author: anton
 */
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

void *_mesa_DummyProgram=NULL;

void
_mesa_destroy_shader_compiler(void)
{
}

/**
 * Releases compiler caches to trade off performance for memory.
 *
 * Intended to be used with glReleaseShaderCompiler().
 */
void
_mesa_destroy_shader_compiler_caches(void)
{
}


struct gl_context;
struct gl_program;

void
_mesa_reference_program_(struct gl_context *ctx, struct gl_program **ptr,
                         struct gl_program *prog)
{

}

struct gl_program_machine;

int
_mesa_execute_program(struct gl_context * ctx,
                      const struct gl_program *program,
                      struct gl_program_machine *machine)
{
	return 0;
}

void
_mesa_init_program(struct gl_context *ctx) {

}

void
_mesa_free_program_data(struct gl_context *ctx)
{
}

void
_mesa_update_default_objects_program(struct gl_context *ctx)
{
}

struct gl_program *
_mesa_new_program(struct gl_context *ctx, int target, uint32_t id,
                  int is_arb_asm)
{
	return NULL;
}

void
_mesa_delete_program(struct gl_context *ctx, struct gl_program *prog)
{
}

struct gl_program *
_mesa_lookup_program(struct gl_context *ctx, uint32_t id)
{
	return NULL;
}

struct gl_program_parameter_list;
void
_mesa_load_state_parameters(struct gl_context *ctx,
                            struct gl_program_parameter_list *paramList)
{
}

void
_mesa_parse_arb_fragment_program(struct gl_context* ctx, int target,
                                 const void *str, size_t len,
                                 struct gl_program *program)
{
}

void
_mesa_parse_arb_vertex_program(struct gl_context *ctx, int target,
			       const void *str, size_t len,
			       struct gl_program *program)
{
}

void
_mesa_print_program(const struct gl_program *prog)
{
}

struct gl_program_cache;

struct gl_program *
_mesa_search_program_cache(struct gl_program_cache *cache,
                           const void *key, uint32_t keysize)
{
	return NULL;
}

void
_mesa_program_cache_insert(struct gl_context *ctx,
                           struct gl_program_cache *cache,
                           const void *key, uint32_t keysize,
                           struct gl_program *program)
{
}

struct gl_program_parameter_list;

struct gl_program_parameter_list *
_mesa_new_parameter_list(void)
{
   return NULL;
}


struct _mesa_glsl_parse_state;
void
_mesa_glsl_initialize_types(struct _mesa_glsl_parse_state *state)
{
}

int
_mesa_add_typed_unnamed_constant(struct gl_program_parameter_list *paramList,
                           const void *values[4], uint32_t size,
                           int datatype, uint32_t *swizzleOut)
{
	return 0;
}

int
_mesa_add_state_reference(struct gl_program_parameter_list *paramList,
                          const int stateTokens[10])
{
	return 0;
}

struct prog_instruction;

struct prog_instruction *
_mesa_copy_instructions(struct prog_instruction *dest,
                        const struct prog_instruction *src, uint32_t n)
{
   return dest;
}

const char *
_mesa_shader_stage_to_string(unsigned stage)
{
	return NULL;
}

struct gl_shader;
void
_mesa_glsl_compile_shader(struct gl_context *ctx, struct gl_shader *shader,
                          int dump_ast, int dump_hir)
{
}

void
_mesa_write_shader_to_file(const struct gl_shader *shader)
{
}

void
_mesa_print_ir(FILE *f, void *instructions,
	       struct _mesa_glsl_parse_state *state)
{
}

struct gl_shader_program;
void
_mesa_glsl_link_shader(struct gl_context *ctx, struct gl_shader_program *prog)
{
}

int
_mesa_ir_link_shader(struct gl_context *ctx, struct gl_shader_program *prog)
{
	return 0;
}


const char *
gl_vert_attrib_name(int attrib)
{
	return NULL;
}

