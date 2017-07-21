/*
 * stubs_cpp.cpp
 *
 *  Created on: Jul 8, 2017
 *      Author: anton
 */


long
parse_program_resource_name(const char *name,
                            const char **out_base_name_end)
{
	return 0;
}


enum ir_node_type {
   ir_type_dereference_array,
   ir_type_dereference_record,
   ir_type_dereference_variable,
   ir_type_constant,
   ir_type_expression,
   ir_type_swizzle,
   ir_type_texture,
   ir_type_variable,
   ir_type_assignment,
   ir_type_call,
   ir_type_function,
   ir_type_function_signature,
   ir_type_if,
   ir_type_loop,
   ir_type_loop_jump,
   ir_type_return,
   ir_type_discard,
   ir_type_emit_vertex,
   ir_type_end_primitive,
   ir_type_barrier,
   ir_type_max, /**< maximum ir_type enum number, for validation */
   ir_type_unset = ir_type_max
};


class ir_rvalue  {
protected:
   ir_rvalue(enum ir_node_type t);
};

ir_rvalue::ir_rvalue(enum ir_node_type t)
{
}


enum ir_texture_opcode {
   ir_tex,		/**< Regular texture look-up */
   ir_txb,		/**< Texture look-up with LOD bias */
   ir_txl,		/**< Texture look-up with explicit LOD */
   ir_txd,		/**< Texture look-up with partial derivatvies */
   ir_txf,		/**< Texel fetch with explicit LOD */
   ir_txf_ms,           /**< Multisample texture fetch */
   ir_txs,		/**< Texture size */
   ir_lod,		/**< Texture lod query */
   ir_tg4,		/**< Texture gather */
   ir_query_levels,     /**< Texture levels query */
   ir_texture_samples,  /**< Texture samples query */
   ir_samples_identical, /**< Query whether all samples are definitely identical. */
};

class ir_texture  {
public:
   ir_texture(enum ir_texture_opcode op)
	{
	}
};
