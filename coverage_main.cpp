#include "./tests/unit_tests.h"
#include "./tests_ffi/test_ffi.h"

int main(int argc, char **argv)
{
    unit_tests();
    ffi_tests();

    return 0;
}
