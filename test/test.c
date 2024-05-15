#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <urdflib.h>
#include <unity.h>

#define about 20
#define communicator 22
#define conveys 23
#define ptime 29
#define rtime 30
#define timestamp 36
#define medium 39
// nouveau
#define featureOI 40
#define result 41
#define evaluates 42
#define madeBy 43

#define PREFIXE_COSDATASET 40
#define PREFIXE_COSWOT 41
#define PREFIXE_SOSA 42

void test_create_uriref()
{
    uint8_t b[1] = {0x06};
    urdflib_t expected = {.buffer = b, .size = 1, .type = TYPE_URIREF};
    urdflib_t actual = urdflib_create_uriref(6);

    TEST_ASSERT_EQUAL(0, urdflib_cmp(&expected, &actual));
}

void test_create_uriref_curie()
{
    uint8_t b[6] = {0xD9, 0x01, 0x40, 0x82, 0x00, 0x00};
    urdflib_t expected = {.buffer = b, .size = 6, .type = TYPE_URIREF};
    urdflib_t actual = urdflib_create_uriref_curie(0, 0);

    TEST_ASSERT_EQUAL(0, urdflib_cmp(&expected, &actual));
}

void test_create_bnode()
{
    uint8_t b[4] = {0xD9, 0x07, 0xE4, 0x00};
    urdflib_t expected = {.buffer = b, .size = 4, .type = TYPE_BNODE};
    urdflib_t actual = urdflib_create_bnode();
    urdflib_t actual_other = urdflib_create_bnode();

    TEST_ASSERT_EQUAL(0, urdflib_cmp(&expected, &actual));

    TEST_ASSERT_EQUAL(0, urdflib_cmp(&actual, &actual_other) == 0);
}

void test_create_literal()
{
    uint8_t b[5] = {0x64, 0x70, 0x6C, 0x6F, 0x70};
    urdflib_t expected = {.buffer = b, .size = 5, .type = TYPE_LITERAL};
    urdflib_t actual = urdflib_create_literal("plop");

    TEST_ASSERT_EQUAL(0, urdflib_cmp(&expected, &actual));
}

void test_create_literal_float()
{
    uint8_t b[5] = {0xFA, 0x40, 0x48, 0xF5, 0xC3};
    urdflib_t expected = {.buffer = b, .size = 5, .type = TYPE_LITERAL};
    urdflib_t actual = urdflib_create_literal_float(3.14);

    TEST_ASSERT_EQUAL(0, urdflib_cmp(&expected, &actual));
}

void test_create_literal_date()
{
    uint8_t b[6] = {0xC1, 0x1A, 0x65, 0xBA, 0x78, 0xEE};
    urdflib_t expected = {.buffer = b, .size = 6, .type = TYPE_LITERAL};
    urdflib_t actual = urdflib_create_literal_date(1706719470);

    TEST_ASSERT_EQUAL(0, urdflib_cmp(&expected, &actual));
}

void test_create_typed_literal()
{
    uint8_t b[9] = {0xA2, 0x03, 0x62, 0x31, 0x32, 0x02, 0x19, 0x01, 0x63};
    urdflib_t expected = {.buffer = b, .size = 9, .type = TYPE_LITERAL};
    urdflib_t dt = urdflib_create_uriref(355);
    urdflib_t actual = urdflib_create_typed_literal("12", &dt);

    TEST_ASSERT_EQUAL(0, urdflib_cmp(&expected, &actual));
}

void test_create_graph()
{
    uint8_t b[5] = {0xBF, 0x01, 0x9F, 0xFF, 0xFF};
    urdflib_t expected = {.buffer = b, .size = 5, .type = TYPE_GRAPH};
    urdflib_t actual = urdflib_create_graph();

    urdflib_freeze(&actual);

    TEST_ASSERT_EQUAL(0, urdflib_cmp(&expected, &actual));
}

void test_create_named_graph()
{
    uint8_t b[12] = {0xBF, 0x00, 0xD9, 0x01, 0x40, 0x82, 0x00, 0x00, 0x01, 0x9F, 0xFF, 0xFF};
    urdflib_t expected = {.buffer = b, .size = 12, .type = TYPE_GRAPH};
    urdflib_t curie = urdflib_create_uriref_curie(0, 0);
    urdflib_t actual = urdflib_create_named_graph(&curie);

    urdflib_freeze(&actual);

    TEST_ASSERT_EQUAL(0, urdflib_cmp(&expected, &actual));
}

void test_add_triple()
{
    uint8_t b[16] = {0xBF, 0x01, 0x9F, 0xBF, 0x00, 0xD9, 0x01, 0x40, 0x82, 0x00, 0x00, 0x06, 0x07, 0xFF, 0xFF, 0xFF};
    urdflib_t expected = {.buffer = b, .size = 16, .type = TYPE_GRAPH};
    urdflib_t s = urdflib_create_uriref_curie(0, 0);
    urdflib_t p = urdflib_create_uriref(6);
    urdflib_t o = urdflib_create_uriref(7);
    urdflib_t actual = urdflib_create_graph();

    urdflib_add_triple(&actual, &s, &p, &o);
    urdflib_freeze(&actual);

    TEST_ASSERT_EQUAL(0, urdflib_cmp(&expected, &actual));
}

void test_add_triples()
{
    uint8_t b[18] = {0xBF, 0x01, 0x9F, 0xBF, 0x00, 0xD9, 0x01, 0x40, 0x82, 0x00, 0x00, 0x06, 0x07, 0x08, 0x09, 0xFF, 0xFF, 0xFF};
    urdflib_t expected = {.buffer = b, .size = 18, .type = TYPE_GRAPH};
    urdflib_t s = urdflib_create_uriref_curie(0, 0);
    urdflib_t p1 = urdflib_create_uriref(6);
    urdflib_t o1 = urdflib_create_uriref(7);
    urdflib_t p2 = urdflib_create_uriref(8);
    urdflib_t o2 = urdflib_create_uriref(9);
    urdflib_t actual = urdflib_create_graph();

    urdflib_add_triple(&actual, &s, &p1, &o1);
    urdflib_add_triple(&actual, &s, &p2, &o2);
    urdflib_freeze(&actual);

    TEST_ASSERT_EQUAL(0, urdflib_cmp(&expected, &actual));
}

void test_add_literals()
{
    uint8_t b[35] = {0xBF, 0x01, 0x9F, 0xBF, 0x00, 0xD9, 0x01, 0x40, 0x82, 0x00, 0x00, 0x08, 0xC1, 0x1A, 0x65, 0xBA, 0x78, 0xEE, 0x06, 0x07, 0x09, 0xFA, 0x40, 0x48, 0xF5, 0xC3, 0x0A, 0x64, 0x70, 0x6C, 0x6F, 0x70, 0xFF, 0xFF, 0xFF};
    urdflib_t expected = {.buffer = b, .size = 35, .type = TYPE_GRAPH};
    urdflib_t s = urdflib_create_uriref_curie(0, 0);
    urdflib_t p1 = urdflib_create_uriref(8);
    urdflib_t o1 = urdflib_create_literal_date(1706719470);
    urdflib_t p2 = urdflib_create_uriref(6);
    urdflib_t o2 = urdflib_create_uriref(7);
    urdflib_t p3 = urdflib_create_uriref(9);
    urdflib_t o3 = urdflib_create_literal_float(3.14);
    urdflib_t p4 = urdflib_create_uriref(10);
    urdflib_t o4 = urdflib_create_literal("plop");
    urdflib_t actual = urdflib_create_graph();

    urdflib_add_triple(&actual, &s, &p1, &o1);
    urdflib_add_triple(&actual, &s, &p2, &o2);
    urdflib_add_triple(&actual, &s, &p3, &o3);
    urdflib_add_triple(&actual, &s, &p4, &o4);
    urdflib_freeze(&actual);

    TEST_ASSERT_EQUAL(0, urdflib_cmp(&expected, &actual));
}

void test_add_tree()
{
    uint8_t b[47] = {0xBF, 0x01, 0x9F, 0xBF, 0x00, 0xD9, 0x01, 0x40, 0x82, 0x00, 0x00, 0x06, 0xD9, 0x01, 0x40, 0x82, 0x00, 0x01, 0x07, 0x08, 0xFF, 0xBF, 0x00, 0xD9, 0x01, 0x40, 0x82, 0x00, 0x01, 0x09, 0xD9, 0x07, 0xE4, 0x02, 0x0B, 0x0C, 0xFF, 0xBF, 0x00, 0x08, 0x09, 0x0D, 0x0E, 0x0F, 0xFF, 0xFF, 0xFF};
    urdflib_t expected = {.buffer = b, .size = 47, .type = TYPE_GRAPH};
    urdflib_t s1 = urdflib_create_uriref_curie(0, 0);
    urdflib_t s2 = urdflib_create_uriref_curie(0, 1);
    urdflib_t s3 = urdflib_create_uriref(8);
    urdflib_t p1 = urdflib_create_uriref(6);
    urdflib_t p2 = urdflib_create_uriref(7);
    urdflib_t p3 = urdflib_create_uriref(9);
    urdflib_t p4 = urdflib_create_uriref(11);
    urdflib_t p5 = urdflib_create_uriref(14);
    urdflib_t o1 = urdflib_create_bnode();
    urdflib_t o2 = urdflib_create_uriref(12);
    urdflib_t o3 = urdflib_create_uriref(13);
    urdflib_t o4 = urdflib_create_uriref(15);
    urdflib_t actual = urdflib_create_graph();

    urdflib_add_triple(&actual, &s1, &p1, &s2);
    urdflib_add_triple(&actual, &s1, &p2, &s3);
    urdflib_add_triple(&actual, &s2, &p3, &o1);
    urdflib_add_triple(&actual, &s2, &p4, &o2);
    urdflib_add_triple(&actual, &s3, &p3, &o3);
    urdflib_add_triple(&actual, &s3, &p5, &o4);
    urdflib_freeze(&actual);

    TEST_ASSERT_EQUAL(0, urdflib_cmp(&expected, &actual));
}

void test_find_next_triple()
{
    uint8_t b[35] = {0xBF, 0x01, 0x9F, 0xBF, 0x00, 0xD9, 0x01, 0x40, 0x82, 0x00, 0x00, 0x08, 0xC1, 0x1A, 0x65, 0xBA, 0x78, 0xEE, 0x06, 0x07, 0x09, 0xFA, 0x40, 0x48, 0xF5, 0xC3, 0x0A, 0x64, 0x70, 0x6C, 0x6F, 0x70, 0xFF, 0xFF, 0xFF};
    urdflib_t g = {.buffer = b, .size = 35, .type = TYPE_GRAPH};
    urdflib_t expected_s = urdflib_create_uriref_curie(0, 0);
    urdflib_t expected_p = urdflib_create_uriref(6);
    urdflib_t expected_o = urdflib_create_uriref(7);
    urdflib_t s, p, o;
    urdflib_ctx_t ctx;
    int status;
    uint8_t expected_count = 4;
    uint8_t actual_count = 0;

    ctx.idx = 0;
    ctx.key_idx = 0;
    ctx.node_idx = 0;

    do
    {
        status = urdflib_find_next_triple(&g, &ctx, &s, &p, &o);
        if (status == STATUS_OK)
            actual_count++;

        if (actual_count == 2)
        {
            TEST_ASSERT_EQUAL(0, urdflib_cmp(&s, &expected_s));
            TEST_ASSERT_EQUAL(0, urdflib_cmp(&p, &expected_p));
            TEST_ASSERT_EQUAL(0, urdflib_cmp(&o, &expected_o));
        }
    } while (status == STATUS_OK);

    TEST_ASSERT_EQUAL(0, expected_count - actual_count);
}

void test_find_in_tree()
{
    uint8_t b[44] = {0xBF, 0x01, 0x9F, 0xBF, 0x00, 0xD9, 0x01, 0x40, 0x82, 0x00, 0x00, 0x06, 0xD9, 0x01, 0x40, 0x82, 0x00, 0x01, 0x07, 0x08, 0xFF, 0xBF, 0x00, 0xD9, 0x01, 0x40, 0x82, 0x00, 0x01, 0x09, 0x0A, 0x0B, 0x0C, 0xFF, 0xBF, 0x00, 0x08, 0x09, 0x0D, 0x0E, 0x0F, 0xFF, 0xFF, 0xFF};
    urdflib_t g = {.buffer = b, .size = 44, .type = TYPE_GRAPH};
    urdflib_t expected_s = urdflib_create_uriref_curie(0, 1);
    urdflib_t expected_p = urdflib_create_uriref(11);
    urdflib_t expected_o = urdflib_create_uriref(12);
    urdflib_t s, p, o;
    urdflib_ctx_t ctx;
    int status;
    uint8_t expected_count = 6;
    uint8_t actual_count = 0;

    ctx.idx = 0;
    ctx.key_idx = 0;
    ctx.node_idx = 0;

    do
    {
        status = urdflib_find_next_triple(&g, &ctx, &s, &p, &o);
        if (status == STATUS_OK)
            actual_count++;

        if (actual_count == 4)
        {
            TEST_ASSERT_EQUAL(0, urdflib_cmp(&s, &expected_s));
            TEST_ASSERT_EQUAL(0, urdflib_cmp(&p, &expected_p));
            TEST_ASSERT_EQUAL(0, urdflib_cmp(&o, &expected_o));
        }
    } while (status == STATUS_OK);

    TEST_ASSERT_EQUAL(0, expected_count - actual_count);
}

void setUp()
{
    // nothing to do
}

void tearDown()
{
    // nothing to do
}

int main()
{
    UNITY_BEGIN();

    RUN_TEST(test_create_uriref);
    RUN_TEST(test_create_uriref_curie);

    RUN_TEST(test_create_bnode);

    RUN_TEST(test_create_literal);
    RUN_TEST(test_create_literal_float);
    RUN_TEST(test_create_literal_date);
    RUN_TEST(test_create_typed_literal);

    RUN_TEST(test_create_graph);
    RUN_TEST(test_create_named_graph);
    RUN_TEST(test_add_triple);
    RUN_TEST(test_add_triples);
    RUN_TEST(test_add_literals);
    RUN_TEST(test_add_tree);

    RUN_TEST(test_find_next_triple);
    RUN_TEST(test_find_in_tree);

    return UNITY_END();
}
