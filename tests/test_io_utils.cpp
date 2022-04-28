#include <unistd.h>
#include <string.h>
#include "./test_io_utils.h"
#include "../src/io_utils.h"

int test_fail_1()
{
    FILE *f = NULL;
    char *r = read_all_f(f);
    ASSERT(r == NULL);
    return 1;
}

int test_eof_1()
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

int test_matching()
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

int test_matching_long()
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

SUB_TEST(io_utils_cpp_test,
{&test_fail_1, "Test fail 1"},
{&test_eof_1, "Test EOF 1"},
{&test_matching, "Test matching"},
{&test_matching_long, "Test matching long"}
        )

