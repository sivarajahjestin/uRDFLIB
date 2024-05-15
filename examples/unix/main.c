#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "../include/urdflib.h"

// first five numbers reserved by @-keywords
#define NAMESPACE_coswot 6
#define NAMESPACE_cosdataset 7
#define NAMESPACE_saref 8
#define TERM_coswot_Communication 9
#define TERM_coswot_hasMedium 10
#define TERM_coswot_hasCommunicator 11
#define TERM_coswot_conveys 12
#define TERM_coswot_isAbout 13
#define TERM_coswot_hasTimestamp 14
#define TERM_saref_Observation 15
#define TERM_saref_madeBy 16
#define TERM_saref_hasResult 17
#define TERM_saref_hasValue 18
#define TERM_saref_resultTime 19

urdflib_t communication;
urdflib_t has_medium;
urdflib_t has_communicator;
urdflib_t conveys;
urdflib_t is_about;
urdflib_t has_timestamp;
urdflib_t observation;
urdflib_t made_by;
urdflib_t has_result;
urdflib_t has_value;
urdflib_t result_time;

int init_vocab()
{
    communication = urdflib_create_uriref(TERM_coswot_Communication);
    has_medium = urdflib_create_uriref(TERM_coswot_hasMedium);
    has_communicator = urdflib_create_uriref(TERM_coswot_hasCommunicator);
    conveys = urdflib_create_uriref(TERM_coswot_conveys);
    is_about = urdflib_create_uriref(TERM_coswot_isAbout);
    has_timestamp = urdflib_create_uriref(TERM_coswot_hasTimestamp);
    observation = urdflib_create_uriref(TERM_saref_Observation);
    made_by = urdflib_create_uriref(TERM_saref_madeBy);
    has_result = urdflib_create_uriref(TERM_saref_hasResult);
    has_value = urdflib_create_uriref(TERM_saref_hasValue);
    result_time = urdflib_create_uriref(TERM_saref_resultTime);
}

int print_count(const urdflib_t *g)
{
    int status, count;
    urdflib_ctx_t ctx;
    urdflib_t s, p, o;

    count = 0;

    ctx.idx = 0;
    ctx.node_idx = 0;
    ctx.key_idx = 0;

    do
    {
        status = urdflib_find_next_triple(g, &ctx, &s, &p, &o);
        if (status == STATUS_OK)
            count++;
        else if (status != STATUS_NO_ITEM)
            count = -1;
    } while (status == STATUS_OK);

    printf("Found %d triples in graph.\n", count);
}

int write_to_file(const char *filename, const urdflib_t *g)
{
    uint8_t *buf = g->buffer;
    size_t len = g->size;
    FILE *fp = fopen(filename, "wb");

    if (fp == NULL)
    {
        printf("File '%s' cannot be opened. Aborting.\n", filename);
        return 1;
    }

    fwrite(buf, 1, len, fp);
    printf("Wrote %lu bytes to file '%s'.\n", len, filename);

    fclose(fp);

    return 0;
}

int main(int argc, char const *argv[])
{
    urdflib_t g = urdflib_create_graph();

    urdflib_t com = urdflib_create_uriref_curie(NAMESPACE_cosdataset, 0);
    urdflib_t cossb = urdflib_create_uriref_curie(NAMESPACE_cosdataset, 1);
    urdflib_t cosio = urdflib_create_uriref_curie(NAMESPACE_cosdataset, 2);
    urdflib_t obs = urdflib_create_uriref_curie(NAMESPACE_cosdataset, 3);
    urdflib_t sensor = urdflib_create_uriref_curie(NAMESPACE_cosdataset, 4);
    urdflib_t res = urdflib_create_bnode();

    urdflib_t str = urdflib_create_literal("4ET_429_sensor1_CO2");
    urdflib_t ts = urdflib_create_literal_date(1666785720);
    urdflib_t val = urdflib_create_literal_float(1250.);

    init_vocab(); // to do only once

    urdflib_add_triple(&g, &com, &RDF_TYPE, &communication);
    urdflib_add_triple(&g, &com, &has_medium, &cossb);
    urdflib_add_triple(&g, &com, &has_communicator, &cosio);
    urdflib_add_triple(&g, &com, &conveys, &obs);
    urdflib_add_triple(&g, &com, &is_about, &str);
    urdflib_add_triple(&g, &com, &has_timestamp, &ts);

    urdflib_add_triple(&g, &obs, &RDF_TYPE, &observation);
    urdflib_add_triple(&g, &obs, &made_by, &sensor);
    urdflib_add_triple(&g, &obs, &has_result, &res);
    urdflib_add_triple(&g, &obs, &result_time, &ts);

    urdflib_add_triple(&g, &res, &has_value, &val);

    urdflib_freeze(&g);

    print_count(&g);
    return write_to_file("coswot.cbor", &g);
}
