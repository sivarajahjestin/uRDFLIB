#include <string.h>
#include <ctype.h>
#include "cbor.h"
#include "urdflib.h"

/**
 * Initial buffer size for graph buffers.
 */
#define BUFFER_SIZE 512

/**
 * JSON-LD keywords
 */
#define KEYWORD_ID 0
#define KEYWORD_GRAPH 1
#define KEYWORD_TYPE 2
#define KEYWORD_VALUE 3
#define KEYWORD_LANGUAGE 4

/**
 * CBOR tag numbers
 */
#define TAG_NB_EPOCH 1
#define TAG_NB_CURIE 320
#define TAG_NB_URI 327
#define TAG_NB_VARIABLE 2019
#define TAG_NB_BNODE 2020

#define TOKEN_ERROR 0
#define TOKEN_UINT 1
#define TOKEN_NEGINT 2
#define TOKEN_BYTE_STRING_START 3
#define TOKEN_BYTE_STRING 4
#define TOKEN_STRING 5
#define TOKEN_STRING_START 6
#define TOKEN_INDEF_ARRAY_START 7
#define TOKEN_ARRAY_START 8
#define TOKEN_INDEF_MAP_START 9
#define TOKEN_MAP_START 10
#define TOKEN_TAG 11
#define TOKEN_FLOAT 12
#define TOKEN_UNDEF 13
#define TOKEN_NULL 14
#define TOKEN_BOOLEAN 15
#define TOKEN_INDEF_BREAK 16

// Temporary workaround while using libcbor
#ifdef ARDUINO
#define CBOR_ENCODE_UINT(X, Y, Z) cbor_encode_uint_urdflib(X, Y, Z)
#define CBOR_ENCODE_TAG(X, Y, Z) cbor_encode_tag_urdflib(X, Y, Z)
#else
#define CBOR_ENCODE_UINT(X, Y, Z) cbor_encode_uint(X, Y, Z)
#define CBOR_ENCODE_TAG(X, Y, Z) cbor_encode_tag(X, Y, Z)
#endif

// TODO cannot be const?
uint8_t RDF_TYPE_BUF[1] = {0x02};
const urdflib_t RDF_TYPE = {.buffer = RDF_TYPE_BUF, .size = 1, .type = TYPE_URIREF};

/**
 * CBOR token pointing to its raw representation
 */
typedef struct
{
    uint8_t *buffer;
    size_t size;
    uint8_t type;
} urdflib_token_t;

void urdflib_print_token(urdflib_token_t *token)
{
    printf("type: %u, size: %lu, buffer: ", token->type, token->size);
    for (size_t i = 0; i < token->size; i++)
        printf("%02X ", token->buffer[i]);
    printf("\n");
}

bool is_curie_tag(const urdflib_token_t *token)
{
    const uint8_t b[3] = {0xD9, 0x01, 0x40};

    return token->type == TOKEN_TAG && token->size == 3 && memcmp(token->buffer, b, 3) == 0;
}

bool is_bnode_tag(const urdflib_token_t *token)
{
    const uint8_t b[3] = {0xD9, 0x07, 0xE4};

    return token->type == TOKEN_TAG && token->size == 3 && memcmp(token->buffer, b, 3) == 0;
}

bool is_epoch_tag(const urdflib_token_t *token)
{
    return token->type == TOKEN_TAG && token->size == 1 && *(token->buffer) == 0xC1;
}

void urdflib_print(const urdflib_t *x)
{
    printf("type: %u, size: %lu, buffer: ", x->type, x->size);
    for (size_t i = 0; i < x->size; i++)
        printf("%02X ", x->buffer[i]);
    printf("\n");
}

bool is_dataset(const urdflib_t *x)
{
    return x->type == TYPE_DATASET;
}

bool is_graph(const urdflib_t *x)
{
    return x->type == TYPE_GRAPH;
}

bool is_mapping(const urdflib_t *x)
{
    return x->type == TYPE_MAPPING;
}

bool is_uriref(const urdflib_t *x)
{
    return x->type == TYPE_URIREF;
}

bool is_bnode(const urdflib_t *x)
{
    return x->type == TYPE_BNODE;
}

bool is_literal(const urdflib_t *x)
{
    return x->type == TYPE_LITERAL;
}

bool is_id_keyword(const urdflib_t *val)
{
    return val->type == TYPE_URIREF && val->size == 1 && *(val->buffer) == 0x00;
}

int urdflib_cmp(const urdflib_t *x, const urdflib_t *y)
{
    if (x->type != y->type)
        return -1;
    else if (x->size != y->size)
        return -2;
    else
        return memcmp(x->buffer, y->buffer, x->size);
}

/*******************************************************************************
 * Functions to decode data from uRDFLib buffers.
 ******************************************************************************/

void set_uint8_token(void *token, uint8_t val)
{
    ((urdflib_token_t *)token)->type = TOKEN_UINT;
}

void set_uint16_token(void *token, uint16_t val)
{
    ((urdflib_token_t *)token)->type = TOKEN_UINT;
}

void set_uint32_token(void *token, uint32_t val)
{
    ((urdflib_token_t *)token)->type = TOKEN_UINT;
}

void set_uint64_token(void *token, uint64_t val)
{
    ((urdflib_token_t *)token)->type = TOKEN_UINT;
}

void set_byte_string_start_token(void *token)
{
    ((urdflib_token_t *)token)->type = TOKEN_BYTE_STRING_START;
}

void set_byte_string_token(void *token, cbor_data buf, uint64_t size)
{
    ((urdflib_token_t *)token)->type = TOKEN_BYTE_STRING;
}

void set_string_token(void *token, cbor_data buf, uint64_t size)
{
    ((urdflib_token_t *)token)->type = TOKEN_STRING;
}

void set_string_start_token(void *token)
{
    ((urdflib_token_t *)token)->type = TOKEN_STRING_START;
}

void set_indef_array_start_token(void *token)
{
    ((urdflib_token_t *)token)->type = TOKEN_INDEF_ARRAY_START;
}

void set_array_start_token(void *token, uint64_t size)
{
    ((urdflib_token_t *)token)->type = TOKEN_ARRAY_START;
}

void set_indef_map_start_token(void *token)
{
    ((urdflib_token_t *)token)->type = TOKEN_INDEF_MAP_START;
}

void set_map_start_token(void *token, uint64_t size)
{
    ((urdflib_token_t *)token)->type = TOKEN_MAP_START;
}

void set_tag_token(void *token, uint64_t tag)
{
    ((urdflib_token_t *)token)->type = TOKEN_TAG;
}

void set_float_token(void *token, float val)
{
    ((urdflib_token_t *)token)->type = TOKEN_FLOAT;
}

void set_double_token(void *token, double val)
{
    ((urdflib_token_t *)token)->type = TOKEN_FLOAT;
}

void set_undefined_token(void *token)
{
    ((urdflib_token_t *)token)->type = TOKEN_UNDEF;
}

void set_null_token(void *token)
{
    ((urdflib_token_t *)token)->type = TOKEN_NULL;
}

void set_boolean_token(void *token, bool val)
{
    ((urdflib_token_t *)token)->type = TOKEN_BOOLEAN;
}

void set_indef_break_token(void *token)
{
    ((urdflib_token_t *)token)->type = TOKEN_INDEF_BREAK;
}

/**
 * Decode the next CBOR token in the input buffer.
 * No memory allocation is done.
 */
int decode_token(const urdflib_t *x, size_t *idx, urdflib_token_t *token)
{
    const struct cbor_callbacks cb = {
        .uint8 = set_uint8_token,
        .uint16 = set_uint16_token,
        .uint32 = set_uint32_token,
        .uint64 = set_uint64_token,
        // TODO callback for negative integers?
        .negint8 = cbor_empty_callbacks.negint8,
        .negint16 = cbor_empty_callbacks.negint16,
        .negint32 = cbor_empty_callbacks.negint32,
        .negint64 = cbor_empty_callbacks.negint64,
        .byte_string_start = set_byte_string_start_token,
        .byte_string = set_byte_string_token,
        .string = set_string_token,
        .string_start = set_string_start_token,
        .indef_array_start = set_indef_array_start_token,
        .array_start = set_array_start_token,
        .indef_map_start = set_indef_map_start_token,
        .map_start = set_map_start_token,
        .tag = set_tag_token,
        .float2 = set_float_token,
        .float4 = set_float_token,
        .float8 = set_double_token,
        .undefined = set_undefined_token,
        .null = set_null_token,
        .boolean = set_boolean_token,
        .indef_break = set_indef_break_token};

    struct cbor_decoder_result res;

    res = cbor_stream_decode(x->buffer + *idx, x->size - *idx, &cb, token);

    if (res.status != CBOR_DECODER_FINISHED)
        return STATUS_CBOR_ERROR;

    token->buffer = x->buffer + *idx;
    token->size = res.read;
    // token's type set by callback

    *idx += token->size;

    return STATUS_OK;
}

int lookup_token(const urdflib_t *x, size_t *idx, urdflib_token_t *token)
{
    int status;
    size_t lookup_idx;

    lookup_idx = *idx;
    status = decode_token(x, &lookup_idx, token);

    return status;
}

int decode_value(const urdflib_t *g, size_t *idx, urdflib_t *val)
{
    int status;
    uint8_t type;
    size_t start_idx;
    urdflib_token_t token;

    status = lookup_token(g, idx, &token);
    if (token.type == TOKEN_INDEF_BREAK)
        return STATUS_NO_ITEM;

    start_idx = *idx;
    status = decode_token(g, idx, &token);

    if (token.type == TOKEN_UINT)
        type = TYPE_URIREF;
    else if (token.type == TOKEN_STRING)
        type = TYPE_LITERAL;
    else if (token.type == TOKEN_FLOAT)
        type = TYPE_LITERAL;
    else if (token.type == TOKEN_TAG)
    {
        if (is_curie_tag(&token))
        {
            decode_token(g, idx, &token);
            if (token.type != TOKEN_ARRAY_START)
                return STATUS_BUFFER_ERROR;
            decode_token(g, idx, &token);
            if (token.type != TOKEN_UINT)
                return STATUS_BUFFER_ERROR;
            decode_token(g, idx, &token);
            if (token.type != TOKEN_UINT)
                return STATUS_BUFFER_ERROR;

            type = TYPE_URIREF;
        }
        else if (is_bnode_tag(&token))
        {
            decode_token(g, idx, &token);
            if (token.type != TOKEN_UINT)
                return STATUS_BUFFER_ERROR;

            type = TYPE_BNODE;
        }
        else if (is_epoch_tag(&token))
        {
            decode_token(g, idx, &token);
            if (token.type != TOKEN_UINT)
                return STATUS_BUFFER_ERROR;

            type = TYPE_LITERAL;
        }

        // TODO variable
    }
    else
        return STATUS_BUFFER_ERROR;

    if (val != NULL)
    {
        val->buffer = g->buffer + start_idx;
        val->size = *idx - start_idx;
        val->type = type;
    }

    return STATUS_OK;
}

int decode_graph_start(const urdflib_t *g, size_t *idx, urdflib_t *id)
{
    int status;
    uint8_t *id_buf;
    size_t id_size;
    urdflib_token_t token;

    if (!is_graph(g) && !is_dataset(g))
        return STATUS_ARG_ERROR;

    // { ... }
    status = decode_token(g, idx, &token);
    if (token.type != TOKEN_INDEF_MAP_START)
        return STATUS_BUFFER_ERROR;

    status = decode_token(g, idx, &token);

    // TODO make @id, @type tokens static
    if (token.type == TOKEN_UINT && *(token.buffer) == 0x00)
    {
        // { @id: ..., ... }
        status = decode_value(g, idx, id);
        // retry above decode_token()
        status = decode_token(g, idx, &token);
    }
    else if (id != NULL)
    {
        id->type = TYPE_URIREF;
        id->size = 0;
        id->buffer = NULL;
    }

    // { ..., @graph: [...] }
    if (token.type != TOKEN_UINT || *(token.buffer) != 0x01)
        return STATUS_BUFFER_ERROR;

    status = decode_token(g, idx, &token);
    if (token.type != TOKEN_INDEF_ARRAY_START)
        return STATUS_BUFFER_ERROR;

    return status;
}

int decode_node_start(const urdflib_t *g, size_t *idx, urdflib_t *id)
{
    int status;
    urdflib_token_t token;

    status = lookup_token(g, idx, &token);
    if (token.type == TOKEN_INDEF_BREAK)
        return STATUS_NO_ITEM;

    // { }
    status = decode_token(g, idx, &token);
    if (token.type != TOKEN_INDEF_MAP_START)
        return STATUS_BUFFER_ERROR;

    // { @id: ..., ... }
    // TODO if blank node, skip
    status = decode_token(g, idx, &token);
    if (token.type != TOKEN_UINT || *(token.buffer) != 0x00)
        return STATUS_BUFFER_ERROR;

    status = decode_value(g, idx, id);

    return status;
}

int decode_key(const urdflib_t *g, size_t *idx, urdflib_t *key)
{
    int status;
    urdflib_token_t token;

    status = lookup_token(g, idx, &token);
    if (token.type == TOKEN_INDEF_BREAK)
        return STATUS_NO_ITEM;

    status = decode_value(g, idx, key);
    // FIXME if key is null, segfault
    if (key->type != TYPE_URIREF)
        return STATUS_BUFFER_ERROR;

    return status;
}

int decode_values_start(const urdflib_t *g, size_t *idx)
{
    // TODO
    return STATUS_OK;
}

int decode_values_end(const urdflib_t *g, size_t *idx)
{
    // TODO
    return STATUS_OK;
}

int decode_pair(const urdflib_t *g, size_t *idx, urdflib_t *key)
{
    int status;

    status = decode_key(g, idx, key);
    if (status < STATUS_OK)
        return status;

    status = decode_values_start(g, idx);
    if (status < STATUS_OK)
        return status;

    do
        status = decode_value(g, idx, NULL);
    while (status == STATUS_OK);

    if (status == STATUS_NO_ITEM)
        status = decode_values_end(g, idx);

    return status;
}

int decode_pairs(const urdflib_t *g, size_t *idx)
{
    urdflib_t key; // not inspected
    int status;

    do
        status = decode_pair(g, idx, &key);
    while (status > 0);

    return status < STATUS_NO_ITEM ? status : STATUS_OK;
}

int decode_node_end(const urdflib_t *g, size_t *idx)
{
    int status;
    urdflib_token_t token;

    status = decode_token(g, idx, &token);
    if (token.type != TOKEN_INDEF_BREAK)
        return STATUS_BUFFER_ERROR;

    return status;
}

int decode_graph_end(const urdflib_t *g, size_t *idx)
{
    int status;
    urdflib_token_t token;

    // { @graph: [ ... ] }
    status = decode_token(g, idx, &token);
    if (token.type != TOKEN_INDEF_BREAK)
        return STATUS_BUFFER_ERROR;

    // { ... }
    status = decode_token(g, idx, &token);
    if (token.type != TOKEN_INDEF_BREAK)
        return STATUS_BUFFER_ERROR;

    return status;
}

/*******************************************************************************
 * Functions to encode data to uRDFLib buffers.
 ******************************************************************************/

int encode_uriref(urdflib_t *g, size_t *idx, uint16_t id)
{
    *idx += CBOR_ENCODE_UINT(id, g->buffer + *idx, g->size);

    return STATUS_OK;
}

int encode_uriref_curie(urdflib_t *g, size_t *idx, uint16_t ns_id, uint16_t local_id)
{
    *idx += CBOR_ENCODE_TAG(TAG_NB_CURIE, g->buffer + *idx, g->size - *idx);
    *idx += cbor_encode_array_start(2, g->buffer + *idx, g->size - *idx);
    *idx += CBOR_ENCODE_UINT(ns_id, g->buffer + *idx, g->size - *idx);
    *idx += CBOR_ENCODE_UINT(local_id, g->buffer + *idx, g->size - *idx);

    return STATUS_OK;
}

int encode_bnode(urdflib_t *g, size_t *idx, uint16_t id)
{
    *idx += CBOR_ENCODE_TAG(TAG_NB_BNODE, g->buffer + *idx, g->size - *idx);
    *idx += CBOR_ENCODE_UINT(id, g->buffer + *idx, g->size - *idx);

    return STATUS_OK;
}

int encode_literal(urdflib_t *g, size_t *idx, const char *str)
{
    size_t len;

    len = strlen(str);
    *idx += cbor_encode_string_start(len, g->buffer + *idx, g->size - *idx);

    memcpy(g->buffer + *idx, str, len);
    *idx += len;

    return STATUS_OK;
}

int encode_literal_float(urdflib_t *g, size_t *idx, float nb)
{
    *idx += cbor_encode_single(nb, g->buffer + *idx, g->size - *idx);

    return STATUS_OK;
}

int encode_literal_date(urdflib_t *g, size_t *idx, uint64_t unix_ts)
{
    *idx += CBOR_ENCODE_TAG(TAG_NB_EPOCH, g->buffer + *idx, g->size - *idx);
    *idx += CBOR_ENCODE_UINT(unix_ts, g->buffer + *idx, g->size - *idx);

    return STATUS_OK;
}

int encode_value(urdflib_t *g, size_t *idx, const urdflib_t *val)
{
    if (!is_uriref(val) && !is_bnode(val) && !is_literal(val))
        return -1;

    memcpy(g->buffer + *idx, val->buffer, val->size);
    *idx += val->size;

    return STATUS_OK;
}

int encode_typed_literal(urdflib_t *g, size_t *idx, const char *lex, const urdflib_t *dtype)
{
    if (!is_uriref(dtype))
        return -1;

    *idx += cbor_encode_map_start(2, g->buffer + *idx, g->size - *idx);

    *idx += CBOR_ENCODE_UINT(KEYWORD_VALUE, g->buffer + *idx, g->size - *idx);
    encode_literal(g, idx, lex);

    *idx += CBOR_ENCODE_UINT(KEYWORD_TYPE, g->buffer + *idx, g->size - *idx);
    encode_value(g, idx, dtype);

    return STATUS_OK;
}

int encode_variable(urdflib_t *g, size_t *idx, uint16_t var_idx)
{
    *idx += CBOR_ENCODE_TAG(TAG_NB_VARIABLE, g->buffer + *idx, g->size - *idx);
    *idx += CBOR_ENCODE_UINT(var_idx, g->buffer + *idx, g->size);

    return STATUS_OK;
}

int encode_id(urdflib_t *g, size_t *idx, const urdflib_t *id)
{
    if (!is_uriref(id) && !is_bnode(id))
        return -1;
    return encode_value(g, idx, id);
}

int encode_key(urdflib_t *g, size_t *idx, const urdflib_t *key)
{
    if (!is_uriref(key))
        return -1;
    return encode_value(g, idx, key);
}

int encode_graph_start(urdflib_t *g, size_t *idx, const urdflib_t *id)
{
    // {...}
    *idx += cbor_encode_indef_map_start(g->buffer + *idx, g->size);

    if (id != NULL)
    {
        // { @id: name }
        *idx += CBOR_ENCODE_UINT(KEYWORD_ID, g->buffer + *idx, g->size - *idx);
        encode_id(g, idx, id);
    }

    // { ..., @graph: [...] }
    *idx += CBOR_ENCODE_UINT(KEYWORD_GRAPH, g->buffer + *idx, g->size - *idx);
    *idx += cbor_encode_indef_array_start(g->buffer + *idx, g->size - *idx);

    *idx += cbor_encode_break(g->buffer + *idx, g->size - *idx);
    *idx += cbor_encode_break(g->buffer + *idx, g->size - *idx);

    return STATUS_OK;
}

int encode_node_start(urdflib_t *g, size_t *idx, const urdflib_t *id)
{
    // encode map start
    *idx += cbor_encode_indef_map_start(g->buffer + *idx, g->size);

    if (id != NULL)
    {
        *idx += CBOR_ENCODE_UINT(KEYWORD_ID, g->buffer + *idx, g->size - *idx);
        encode_id(g, idx, id);
    }

    return STATUS_OK;
}

int encode_node_end(urdflib_t *g, size_t *idx)
{
    *idx += cbor_encode_break(g->buffer + *idx, g->size - *idx);

    return STATUS_OK;
}

int encode_graph_end(urdflib_t *g, size_t *idx)
{
    *idx += cbor_encode_break(g->buffer + *idx, g->size - *idx);
    *idx += cbor_encode_break(g->buffer + *idx, g->size - *idx);

    return STATUS_OK;
}

/*******************************************************************************
 * Main functions of the uRDFLib module.
 ******************************************************************************/

urdflib_t urdflib_create_uriref(uint16_t id)
{
    uint8_t buf[4];
    size_t idx;
    urdflib_t uriref;

    uriref.buffer = buf;
    uriref.size = 4;
    uriref.type = TYPE_URIREF;

    idx = 0;
    encode_uriref(&uriref, &idx, id);

    // to avoid successive malloc/realloc, temp buffer is used to encode val
    // TODO predict size beforehand
    uriref.buffer = malloc(idx);
    memcpy(uriref.buffer, buf, idx);
    uriref.size = idx;

    return uriref;
}

urdflib_t urdflib_create_uriref_curie(uint16_t ns_id, uint16_t local_id)
{
    uint8_t buf[20];
    size_t idx;
    urdflib_t uriref;

    uriref.buffer = buf;
    uriref.size = 20;
    uriref.type = TYPE_URIREF;

    idx = 0;
    encode_uriref_curie(&uriref, &idx, ns_id, local_id);

    uriref.buffer = malloc(idx);
    memcpy(uriref.buffer, buf, idx);
    uriref.size = idx;

    return uriref;
}

urdflib_t urdflib_create_bnode()
{
    static uint16_t counter = 0;

    uint8_t buf[10];
    size_t idx;
    urdflib_t bnode;

    bnode.buffer = buf;
    bnode.size = 10;
    bnode.type = TYPE_BNODE;

    idx = 0;
    encode_bnode(&bnode, &idx, counter++);

    bnode.buffer = malloc(idx);
    memcpy(bnode.buffer, buf, idx);
    bnode.size = idx;

    return bnode;
}

urdflib_t urdflib_create_literal(const char *str)
{
    size_t len;
    size_t idx;
    urdflib_t lit;

    len = strlen(str);

    // size to allocate: 1 byte for CBOR head + string length
    // FIXME unless len >= 24
    lit.buffer = malloc(1 + len);
    lit.size = 1 + len;
    lit.type = TYPE_LITERAL;

    idx = 0;
    encode_literal(&lit, &idx, str);

    return lit;
}

urdflib_t urdflib_create_literal_float(float nb)
{
    uint8_t buf[6];
    size_t idx;
    urdflib_t lit;

    lit.buffer = buf;
    lit.size = 6;
    lit.type = TYPE_LITERAL;

    idx = 0;
    encode_literal_float(&lit, &idx, nb);

    lit.buffer = malloc(idx);
    memcpy(lit.buffer, buf, idx);
    lit.size = idx;

    return lit;
}

urdflib_t urdflib_create_literal_date(uint64_t unix_ts)
{
    uint8_t buf[6];
    size_t idx;
    urdflib_t lit;

    lit.buffer = buf;
    lit.size = 6;
    lit.type = TYPE_LITERAL;

    idx = 0;
    encode_literal_date(&lit, &idx, unix_ts);

    lit.buffer = malloc(idx);
    memcpy(lit.buffer, buf, idx);
    lit.size = idx;

    return lit;
}

urdflib_t urdflib_create_typed_literal(const char *lex, const urdflib_t *dtype)
{
    size_t len;
    size_t idx;
    urdflib_t lit;

    // TODO 10 is a magic number here
    // should instead first encode dtype, then allocate.
    len = strlen(lex) + 10;

    lit.buffer = malloc(len);
    lit.size = len;
    lit.type = TYPE_LITERAL;

    idx = 0;
    encode_typed_literal(&lit, &idx, lex, dtype);

    lit.buffer = realloc(lit.buffer, idx);
    // TODO check buffer isn't NULL
    lit.size = idx;

    return lit;
}

urdflib_t urdflib_create_variable(uint16_t var_idx)
{
    uint8_t buf[6];
    size_t idx;
    urdflib_t var;

    var.buffer = buf;
    var.size = 6;
    var.type = TYPE_VARIABLE;

    idx = 0;
    encode_variable(&var, &idx, var_idx);

    var.buffer = malloc(idx);
    memcpy(var.buffer, buf, idx);
    var.size = idx;

    return var;
}

urdflib_t urdflib_create_graph()
{
    size_t idx;
    urdflib_t g;

    g.buffer = malloc(BUFFER_SIZE);
    g.size = BUFFER_SIZE;
    g.type = TYPE_GRAPH;

    idx = 0;
    encode_graph_start(&g, &idx, NULL);

    return g;
}

urdflib_t urdflib_create_named_graph(const urdflib_t *name)
{
    size_t idx;
    urdflib_t g;

    g.buffer = malloc(BUFFER_SIZE);
    g.size = BUFFER_SIZE;
    g.type = TYPE_GRAPH;

    idx = 0;
    encode_graph_start(&g, &idx, name);

    return g;
}

int urdflib_add_triple(urdflib_t *g, const urdflib_t *s, const urdflib_t *p, const urdflib_t *o)
{
    int status;
    size_t idx;
    urdflib_t id;
    bool node_found, node_first, node_last;

    if (!is_graph(g))
        return STATUS_ARG_ERROR;
    if (!is_uriref(s) && !is_bnode(s))
        return STATUS_ARG_ERROR;
    if (!is_uriref(p))
        return STATUS_ARG_ERROR;
    if (!is_uriref(s) && !is_bnode(s) && !is_literal(s))
        return STATUS_ARG_ERROR;

    status = 0;
    idx = 0;
    node_found = false;
    node_first = true;
    node_last = true;

    status = decode_graph_start(g, &idx, NULL);

    do
    {
        status = decode_node_start(g, &idx, &id);

        if (status == STATUS_OK && urdflib_cmp(s, &id) == 0)
            node_found = true;

        if (status == STATUS_OK)
            status = decode_pairs(g, &idx);
        if (status == STATUS_OK && !node_found)
            status = decode_node_end(g, &idx);
    } while (status == STATUS_OK && !node_found);

    if (!node_found)
        status = encode_node_start(g, &idx, s);
    // TODO missing break if not first node
    else if (decode_node_start(g, &idx, &id) == STATUS_OK)
    {
        // if node found and not last, interleave
        // TODO realloc
        node_last = false;
        return -100;
    }

    status = encode_key(g, &idx, p);
    status = encode_value(g, &idx, o);

    status = encode_node_end(g, &idx);

    if (node_last)
        status = encode_graph_end(g, &idx);
    else
    {
        // TODO copy rest of nodes to buffer
    }

    return status;
}

int find_node(const urdflib_t *g, urdflib_ctx_t *ctx, urdflib_t *id)
{
    int status;
    size_t node_idx;

    if (!is_graph(g))
        STATUS_ARG_ERROR;

    if (ctx->idx == 0)
        status = decode_graph_start(g, &(ctx->idx), NULL);

    if (ctx->node_idx == 0)
    {
        ctx->node_idx = ctx->idx;
        status = decode_node_start(g, &(ctx->idx), id);
    }
    else
    {
        node_idx = ctx->node_idx;
        status = decode_node_start(g, &node_idx, id);
    }

    return status;
}

int find_key(const urdflib_t *g, urdflib_ctx_t *ctx, urdflib_t *key)
{
    int status;
    size_t key_idx;

    if (!is_graph(g))
        STATUS_ARG_ERROR;

    if (ctx->key_idx == 0)
    {
        ctx->key_idx = ctx->idx;
        status = decode_key(g, &(ctx->idx), key);

        if (status == STATUS_NO_ITEM)
            decode_node_end(g, &(ctx->idx));
    }
    else
    {
        key_idx = ctx->key_idx;
        status = decode_key(g, &key_idx, key);
    }

    return status;
}

int find_value(const urdflib_t *g, urdflib_ctx_t *ctx, urdflib_t *val)
{
    int status;
    size_t idx;

    if (!is_graph(g))
        STATUS_ARG_ERROR;

    // TODO support multiple values
    ctx->has_single_value = true;

    // TODO return error if break not found
    // status = decode_values_start(g, &(ctx->idx));

    status = decode_value(g, &(ctx->idx), val);

    if (!ctx->has_single_value)
        status = decode_values_end(g, &(ctx->idx));

    return status;
}

int urdflib_find_next_triple(const urdflib_t *g, urdflib_ctx_t *ctx, urdflib_t *s, urdflib_t *p, urdflib_t *o)
{
    int status;

    if (!is_graph(g))
        STATUS_ARG_ERROR;

    status = find_node(g, ctx, s);
    if (status != STATUS_OK)
        return status;

    status = find_key(g, ctx, p);
    if (status != STATUS_OK && status != STATUS_NO_ITEM)
        return status;

    if (status == STATUS_NO_ITEM)
    {
        ctx->node_idx = 0;
        ctx->key_idx = 0;
        return urdflib_find_next_triple(g, ctx, s, p, o);
    }

    status = find_value(g, ctx, o);
    if (status != STATUS_OK && status != STATUS_NO_ITEM)
        return status;

    if (ctx->has_single_value)
        ctx->key_idx = 0;
    else if (status == STATUS_NO_ITEM)
    {
        ctx->key_idx = 0;
        return urdflib_find_next_triple(g, ctx, s, p, o);
    }

    return status;
}

void urdflib_freeze(urdflib_t *x)
{
    int status;
    size_t idx;

    if (is_graph(x))
    {
        idx = 0;
        status = decode_graph_start(x, &idx, NULL);

        while (status == STATUS_OK)
        {
            status = decode_node_start(x, &idx, NULL);
            if (status < STATUS_OK)
                break;

            status = decode_pairs(x, &idx);
            if (status < STATUS_OK)
                break;

            status = decode_node_end(x, &idx);
        }

        if (status == STATUS_NO_ITEM)
            status = decode_graph_end(x, &idx);

        // TODO memory leak if error status
        x->buffer = realloc(x->buffer, idx);
        x->size = idx;
    }
}

void urdflib_delete(urdflib_t *x)
{
    free(x->buffer);
    x->size = 0;
}