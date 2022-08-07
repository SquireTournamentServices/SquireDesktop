#include <unistd.h>
#include <string.h>
#include "./test_utils.h"
#include "../src/utils.h"

static int test_fail_1()
{
    FILE *f = NULL;
    char *r = read_all_f(f);
    ASSERT(r == NULL);
    return 1;
}

static int test_eof_1()
{
    int fid[2];
    ASSERT(pipe(fid) == 0);

    FILE *r = fdopen(fid[0], "rb"), *w = fdopen(fid[1], "wb");
    ASSERT(fclose(w) == 0);

    char *ret = read_all_f(r);
    ASSERT(ret != NULL);
    free(ret);

    ASSERT(fclose(r) == 0);

    return 1;
}

static int test_matching()
{

    int fid[2];
    ASSERT(pipe(fid) == 0);

    FILE *r = fdopen(fid[0], "rb"), *w = fdopen(fid[1], "wb");
    char m[DEFAULT_BLOCK_SIZE - 1];
    for (int i = 0; i < sizeof(m); i++) {
        m[i] = i & 0xFF;
        fputc(m[i], w);
    }
    ASSERT(fclose(w) == 0);

    char *ret = read_all_f(r);
    ASSERT(ret != NULL);
    ASSERT(memcmp(ret, m, sizeof(m)) == 0);
    ASSERT(ret[sizeof(m)] == 0);
    free(ret);

    ASSERT(fclose(r) == 0);

    return 1;
}

static int test_matching_long()
{

    int fid[2];
    ASSERT(pipe(fid) == 0);

    FILE *r = fdopen(fid[0], "rb"), *w = fdopen(fid[1], "wb");
    char m[DEFAULT_BLOCK_SIZE * 10];
    for (int i = 0; i < sizeof(m); i++) {
        m[i] = i & 0xFF;
        fputc(m[i], w);
    }
    ASSERT(fclose(w) == 0);

    char *ret = read_all_f(r);
    ASSERT(ret != NULL);
    ASSERT(memcmp(ret, m, sizeof(m)) == 0);
    ASSERT(ret[sizeof(m)] == 0);
    free(ret);

    ASSERT(fclose(r) == 0);

    return 1;
}

#define LOWER_STRING "asdf"
#define UPPER_STRING "ASDF"
static int test_to_lower_case()
{
    std::string str = UPPER_STRING;
    toLowerCase(str);
    ASSERT(LOWER_STRING == str);
}

static int test_to_upper_case()
{
    std::string str = LOWER_STRING;
    toUpperCase(str);
    ASSERT(UPPER_STRING == str);
}

SUB_TEST(utils_cpp_test,
{&test_fail_1, "Test fail 1"},
{&test_eof_1, "Test EOF 1"},
{&test_matching, "Test matching"},
{&test_matching_long, "Test matching long"},
{&test_to_lower_case, "Test to lower case"},
{&test_to_upper_case, "Test to upper case"}
        )

