#include <Arduino.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "urdflib.h"

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

// TODO add fn to create @type
urdflib_t type;
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

void init_vocab()
{
    uint8_t b[1] = {0x02};
    urdflib_t x = {.buffer = b, .size = 1, .type = TYPE_URIREF};

    type = x; // TODO add function to module instead

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

    Serial.print("Found ");
    Serial.print(count);
    Serial.println(" triples in graph.");
    return count;
}

int print_graph(const urdflib_t *g)
{
    uint8_t *buf = g->buffer;
    size_t len = g->size;

    Serial.println("CBOR: ");
    for (size_t i = 0; i < len; i++)
    {
        char buffer[3];
        sprintf(buffer, "%02X", buf[i]);
        Serial.print(buffer);
    }
    Serial.println();

    return 0;
}

void setup()
{
    Serial.begin(115200);

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

    urdflib_add_triple(&g, &com, &type, &communication);
    urdflib_add_triple(&g, &com, &has_medium, &cossb);
    urdflib_add_triple(&g, &com, &has_communicator, &cosio);
    urdflib_add_triple(&g, &com, &conveys, &obs);
    urdflib_add_triple(&g, &com, &is_about, &str);
    urdflib_add_triple(&g, &com, &has_timestamp, &ts);

    urdflib_add_triple(&g, &obs, &type, &observation);
    urdflib_add_triple(&g, &obs, &made_by, &sensor);
    urdflib_add_triple(&g, &obs, &has_result, &res);
    urdflib_add_triple(&g, &obs, &result_time, &ts);

    urdflib_add_triple(&g, &res, &has_value, &val);

    urdflib_freeze(&g);

    delay(2000);
    print_graph(&g);
    print_count(&g);
}

void loop() {}
