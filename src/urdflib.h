#ifdef __cplusplus
extern "C"
{
#endif

#ifndef HEADER_URDFLIB
#define HEADER_URDFLIB

/**
 * @file
 * @brief Main uRDFLib namespace
 */
#include <stdint.h>
#include <stdbool.h>

    // TODO current impl limits ctx size to 2^16. Make it configurable.

#define TYPE_DATASET 0
#define TYPE_GRAPH 1
#define TYPE_MAPPING 2
#define TYPE_URIREF 3
#define TYPE_BNODE 4
#define TYPE_LITERAL 5
#define TYPE_VARIABLE 6

/**
 * Possibls status codes returned by uRDFLib buffer management functions.
 */
#define STATUS_OK 0
#define STATUS_NO_ITEM -1
#define STATUS_CBOR_ERROR -2
#define STATUS_BUFFER_ERROR -3
#define STATUS_ARG_ERROR -4
#define STATUS_MALLOC_ERROR -5

    /**
     * Generic buffer encoding either:
     * - a graph (or graph pattern),
     * - a dataset,
     * - a mapping (the result of evaluating a graph pattern against a graph/dataset) or
     * - an RDF node (URI reference, blank node or literal).
     */
    typedef struct
    {
        uint8_t *buffer;
        size_t size;
        uint8_t type;
    } urdflib_t;

    /**
     * URIRef representing rdf:type.
     */
    extern const urdflib_t RDF_TYPE;

    /**
     * Buffer encoding contextual information used while iterating over a graph.
     */
    typedef struct
    {
        size_t idx;
        size_t node_idx;
        size_t key_idx;
        bool has_single_value;
    } urdflib_ctx_t;

    /**
     * Print a readable representation of buffer x (for debugging purposes).
     *
     * @param[in] x a buffer
     */
    void urdflib_print(const urdflib_t *x);

    /**
     * Compare two uRDFLib buffers.
     *
     * @param[in] x a buffer
     * @param[in] y another buffer
     */
    int urdflib_cmp(const urdflib_t *x, const urdflib_t *y);

    /**
     * Free any extra memory allocated for buffer x.
     *
     * @param[inout] x a buffer
     */
    void urdflib_freeze(urdflib_t *x);

    /**
     * Free all memory allocated for buffer x.
     *
     * @param[in] x a buffer
     */
    void urdflib_delete(urdflib_t *x);

    /**
     * Create an empty anonymous graph.
     */
    urdflib_t urdflib_create_graph();

    /**
     * Create an empty named graph.
     *
     * @param[in] name the graph name represented as a CURIE
     */
    urdflib_t urdflib_create_named_graph(const urdflib_t *name);

    /**
     * Create a URIRef represented as a term index.
     */
    urdflib_t urdflib_create_uriref(uint16_t id);

    /**
     * Create a URIRef represented as a CURIE.
     */
    urdflib_t urdflib_create_uriref_curie(uint16_t ns_id, uint16_t local_id);

    /**
     * Create a BNode with some auto-generated identifier.
     */
    urdflib_t urdflib_create_bnode();

    /**
     * Create a plain literal with a string lexical representation.
     *
     * @param[in] str a string
     */
    urdflib_t urdflib_create_literal(const char *str);

    /**
     * Create a float literal.
     *
     * @param[in] nb a single-precision floating point number
     */
    urdflib_t urdflib_create_literal_float(float nb);

    /**
     * Create a date literal given as Unix (epoch) timestamp.
     *
     * @param[in] unix_ts a Unix timestamp (as long integer)
     */
    urdflib_t urdflib_create_literal_date(uint64_t unix_ts);

    /**
     * Create a typed literal, combining a lexical representation and a datatype.
     *
     * @param[in] lex a lexical representation (as string)
     * @param[in] dtype a datatype (as URIRef)
     */
    urdflib_t urdflib_create_typed_literal(const char *lex, const urdflib_t *dtype);

    /**
     * Create a variable identified by an integer value
     * (which will become an index in some mapping).
     *
     * @param[in] var_idx an integer value
     */
    urdflib_t urdflib_create_variable(uint16_t var_idx);

    /**
     * Add a triple to the given graph.
     *
     * @param[inout] g the graph that will include the added triple
     * @param[in] s the subject of the triple
     * @param[in] p the predicate of the triple
     * @param[in] o the object of the triple
     * @return an error code or 0 if the triple was successfully added
     */
    int urdflib_add_triple(urdflib_t *g, const urdflib_t *s, const urdflib_t *p, const urdflib_t *o);

    urdflib_t urdflib_create_dataset();

    int urdflib_add_graph(urdflib_t *dataset, const uint8_t *graph);

    /**
     * Scan the input graph until a triple is found.
     * The input context acts as an iterator:
     * if the same context is passed twice, a different triple
     * will be found the second time.
     *
     * @param[in] g the graph to iterate over
     * @param[inout] ctx an opaque buffer for contextual information
     * @param[out] s the subject of the next triple found
     * @param[out] p the predicate of the next triple found
     * @param[out] o the object of the next triple found
     * @return a status code
     */
    int urdflib_find_next_triple(const urdflib_t *g, urdflib_ctx_t *ctx, urdflib_t *s, urdflib_t *p, urdflib_t *o);

    int urdflib_find_next_quad(const urdflib_t *ds, urdflib_ctx_t *ctx, urdflib_t *s, urdflib_t *p, urdflib_t *o, urdflib_t *g);

    int urdflib_find_next_mapping(const urdflib_t *g, urdflib_ctx_t *ctx, urdflib_t *q, urdflib_t *mu);

#endif

#ifdef __cplusplus
}
#endif