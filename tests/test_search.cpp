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

    size_t len = res->size();
    for (size_t i = 0; i < len; i++) {
        Card card = res->at(i);

        ASSERT(card.name() != "");
        card.oracle();
        ASSERT(card.types() != "");
        card.cmc();
        card.power();
        card.toughness();
        std::list<Set> sets = card.sets();

        ASSERT(sets.size() != 0);
        for (Set set : sets) {
            ASSERT(set.code() != "");
            ASSERT(set.name() != "");
            set.release();
        }
    }

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
{&test_search_query_success, "Test good query"},
{&test_search_query_bad_throws_exception, "Test bad query"})
