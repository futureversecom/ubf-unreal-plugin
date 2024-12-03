#pragma once

#include <cstdint>
#include <cstdlib>
#include <new>

namespace UBF::FFI {

enum class DynamicType {
  Unknown = 0,
  Bool = 1,
  Int = 2,
  Float = 3,
  String = 4,
  Array = 5,
  Dictionary = 6,
  Foreign = 7,
};

/// A wrapper around ExecutionContext that can safely be moved out of rust.
/// The caller must free this object when done.
struct ArcExecutionContext;

struct Dynamic;

struct GraphInstance;

/// TODO Expensive to clone, move to Arc in future? Ask Travers & Felix
struct NodeRegistry;

using CompletionID = uint32_t;

extern "C" {

GraphInstance *graph_load(NodeRegistry *registry, const uint16_t *json, int32_t json_len);

bool graph_version(GraphInstance *graph, const uint16_t **out_ptr, uintptr_t *out_len);

void graph_iter_inputs(GraphInstance *graph, intptr_t context, bool (*iterator)(intptr_t,
                                                                                const uint8_t*,
                                                                                int32_t,
                                                                                const uint8_t*,
                                                                                int32_t,
                                                                                Dynamic*));

void graph_iter_outputs(GraphInstance *graph, intptr_t context, bool (*iterator)(intptr_t,
                                                                                 const uint8_t*,
                                                                                 int32_t,
                                                                                 const uint8_t*,
                                                                                 int32_t,
                                                                                 Dynamic*));

/// Execute the UBF Graph.
ArcExecutionContext *graph_execute(GraphInstance *graph,
                                   Dynamic *inputs,
                                   Dynamic *context_data,
                                   void (*on_complete)(Dynamic*));

/// Free graph
void graph_release(GraphInstance *graph);

/// Retain graph reference
GraphInstance *graph_retain(GraphInstance *graph);

void ctx_release(ArcExecutionContext *execution_context);

/// Retain reference
ArcExecutionContext *ctx_retain(ArcExecutionContext *execution_context);

bool ctx_trigger_node(ArcExecutionContext *execution_context,
                      const uint16_t *source_node_id_str,
                      int32_t source_node_id_len,
                      const uint16_t *source_port_key_str,
                      int32_t source_port_key_len);

bool ctx_complete_node(ArcExecutionContext *execution_context, CompletionID completion_id);

bool ctx_read_output(ArcExecutionContext *execution_context,
                     const uint16_t *binding_id,
                     int32_t binding_id_len,
                     Dynamic **out);

bool ctx_read_input(ArcExecutionContext *execution_context,
                    const uint16_t *node_id,
                    int32_t node_id_len,
                    const uint16_t *port_key,
                    int32_t port_key_len,
                    Dynamic **out);

Dynamic *ctx_get_context_data(ArcExecutionContext *execution_context);

bool ctx_get_dynamic_data_entry(ArcExecutionContext *execution_context,
                                const uint16_t *key,
                                int32_t key_len,
                                Dynamic **out);

bool ctx_set_dynamic_data_entry(ArcExecutionContext *execution_context,
                                const uint16_t *key,
                                int32_t key_len,
                                Dynamic *value);

bool ctx_get_declared_node_inputs(ArcExecutionContext *execution_context,
                                  const uint16_t *node_id,
                                  int32_t node_id_len,
                                  intptr_t context,
                                  bool (*iterator)(intptr_t, const uint8_t*, int32_t));

bool ctx_get_graph_outputs(ArcExecutionContext *execution_context,
                           intptr_t context,
                           bool (*iterator)(intptr_t, const uint8_t*, int32_t, Dynamic*));

/// Write a value to the current execution context.
void ctx_write_output(ArcExecutionContext *execution_context,
                      const uint16_t *node_id,
                      int32_t node_id_len,
                      const uint16_t *port_key,
                      int32_t port_key_len,
                      Dynamic *value);

void dynamic_release(Dynamic *value);

/// Retain reference
Dynamic *dynamic_retain(Dynamic *value);

Dynamic *dynamic_new_primitive(DynamicType typ, int32_t value, float value_float);

Dynamic *dynamic_new_string(const uint16_t *value, int32_t value_len);

Dynamic *dynamic_new_foreign(intptr_t ptr, void (*drop)(intptr_t));

Dynamic *dynamic_new_dictionary();

Dynamic *dynamic_new_array();

/// Push value onto array
bool dynamic_array_push(Dynamic *array, Dynamic *value);

void dynamic_array_iter(Dynamic *array, intptr_t context, bool (*iterator)(intptr_t, Dynamic*));

/// Return the string value of a dynamic value.
/// Note that we allocate as much memory as needed to store the string and the
/// caller is responsible for freeing it.
bool dynamic_as_string(Dynamic *value, void **out, const uint16_t **out_ptr, uintptr_t *out_len);

void box_release(void *ptr);

bool dynamic_to_string(Dynamic *value, void **out, const uint16_t **out_ptr, uintptr_t *out_len);

void dynamic_extract(Dynamic *value,
                     DynamicType *out_type,
                     int32_t *out_int,
                     float *out_float,
                     intptr_t *out_ptr);

bool dynamic_dictionary_get(Dynamic *dictionary,
                            const uint16_t *key,
                            int32_t key_len,
                            Dynamic **out);

bool dynamic_dictionary_set(Dynamic *dictionary,
                            const uint16_t *key,
                            int32_t key_len,
                            Dynamic *value);

NodeRegistry *registry_new();

/// Free registry
void registry_release(NodeRegistry *registry);

/// Retain reference
NodeRegistry *registry_retain(NodeRegistry *registry);

/// Register a custom, external node.
void registry_register_node(NodeRegistry *registry,
                            const uint16_t *name,
                            int32_t name_len,
                            intptr_t this_ptr,
                            void (*execute_cb)(intptr_t,
                                               const char*,
                                               CompletionID,
                                               ArcExecutionContext*),
                            void (*_release_cb)(intptr_t));

extern void _UnityPluginLoad(void*);

extern void _UnityPluginUnload();

extern void _log(const char *msg);

extern void _log_warn(const char *msg);

extern void _log_error(const char *msg);

/// This function is called when the plugin is loaded.
///
/// note: For reasons the C symbol is not exported after building; hence the
/// wrapper function to forward the call.
void UnityPluginLoad(void *ptr);

/// This function is called when the plugin is unloaded.
///
/// note: For reasons the C symbol is not exported after building; hence the
/// wrapper function to forward the call.
void UnityPluginUnload();

void init_ue_logger(void (*log)(const char *msg));

} // extern "C"

} // namespace UBF::FFI
