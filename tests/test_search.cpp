#include "./test_search.h"
#include "../testing_h/testing.h"
#include "../src/search.h"
#include <exception>

static int test_search_init()
{
    MTGSearchEngine::get_instance();
    return 1;
}

static int test_search_query_success()
{
    SearchResult *res = MTGSearchEngine::get_instance().search("goblin motivator");
    ASSERT(res != NULL);
    delete res;
    return 1;
}

static int test_search_query_bad_throws_exception()
{
    try {
        MTGSearchEngine::get_instance().search("poo:");
    } catch(std::exception &e) {
        return 1;
    }

    lprintf(LOG_ERROR, "No exception was thrown with bad query");
    return 0;
}

SUB_TEST(test_search, {&test_search_init, "Init search"},
{&test_search_query_success, "Test good query"})
