#include <squire_core.h>
#include "./sq_link_test.h"
#include "../src/ffi_utils.h"
#include "../testing_h/testing.h"
#include <string.h>

int sq_link_test()
{
    squire_core::sc_TournamentId tid;
    memset(tid._0, 0, sizeof(tid._0));
    ASSERT(is_null_id(tid._0));

    squire_core::sc_PlayerId pid;
    memset(pid._0, 0, sizeof(pid._0));
    ASSERT(is_null_id(pid._0));

    return 1;
}

