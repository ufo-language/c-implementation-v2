#include "data/hashtable.h"

void ns_any_defineAll(struct D_HashTable* env);
void ns_array_defineAll(struct D_HashTable* env);
void ns_binding_defineAll(struct D_HashTable* env);
void ns_etor_defineAll(struct D_HashTable* env);
void ns_event_defineAll(struct D_HashTable* env);
void ns_ffi_defineAll(struct D_HashTable* env);
void ns_file_defineAll(struct D_HashTable* env);
void ns_globals_defineAll(struct D_HashTable* env);
void ns_hash_defineAll(struct D_HashTable* env);
void ns_integer_defineAll(struct D_HashTable* env);
void ns_io_defineAll(struct D_HashTable* env);
void ns_iterator_defineAll(struct D_HashTable* env);
void ns_json_defineAll(struct D_HashTable* env);
void ns_lexer_defineAll(struct D_HashTable* env);
void ns_list_defineAll(struct D_HashTable* env);
void ns_math_defineAll(struct D_HashTable* env);
void ns_ns_defineAll(struct D_HashTable* env);
void ns_os_defineAll(struct D_HashTable* env);
void ns_parser_defineAll(struct D_HashTable* env);
void ns_queue_defineAll(struct D_HashTable* env);
void ns_rand_defineAll(struct D_HashTable* env);
void ns_real_defineAll(struct D_HashTable* env);
void ns_record_defineAll(struct D_HashTable* env);
void ns_sequence_defineAll(struct D_HashTable* env);
void ns_set_defineAll(struct D_HashTable* env);
void ns_stream_defineAll(struct D_HashTable* env);
void ns_string_defineAll(struct D_HashTable* env);
void ns_symbol_defineAll(struct D_HashTable* env);
void ns_thread_defineAll(struct D_HashTable* env);
void ns_type_defineAll(struct D_HashTable* env);
void ns_ufo_defineAll(struct D_HashTable* env);

struct D_HashTable* _globalEnv;

struct D_HashTable* ns_all_globalEnv(void) {
    return _globalEnv;
}

#include <stdio.h>
void ns_all_rootObjects(void) {
    _globalEnv = hashTable_new();
    ns_any_defineAll(_globalEnv);
    ns_array_defineAll(_globalEnv);
    ns_binding_defineAll(_globalEnv);
    ns_etor_defineAll(_globalEnv);
    ns_event_defineAll(_globalEnv);
    ns_ffi_defineAll(_globalEnv);
    ns_file_defineAll(_globalEnv);
    ns_globals_defineAll(_globalEnv);
    ns_hash_defineAll(_globalEnv);
    ns_integer_defineAll(_globalEnv);
    ns_io_defineAll(_globalEnv);
    ns_iterator_defineAll(_globalEnv);
    ns_json_defineAll(_globalEnv);
    ns_lexer_defineAll(_globalEnv);
    ns_list_defineAll(_globalEnv);
    ns_math_defineAll(_globalEnv);
    ns_ns_defineAll(_globalEnv);
    ns_os_defineAll(_globalEnv);
    ns_parser_defineAll(_globalEnv);
    ns_queue_defineAll(_globalEnv);
    ns_rand_defineAll(_globalEnv);
    ns_real_defineAll(_globalEnv);
    ns_record_defineAll(_globalEnv);
    ns_sequence_defineAll(_globalEnv);
    ns_set_defineAll(_globalEnv);
    ns_stream_defineAll(_globalEnv);
    ns_string_defineAll(_globalEnv);
    ns_symbol_defineAll(_globalEnv);
    ns_thread_defineAll(_globalEnv);
    ns_type_defineAll(_globalEnv);
    ns_ufo_defineAll(_globalEnv);
}
