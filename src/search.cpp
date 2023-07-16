#include "./search.h"
#include "../testing_h/testing.h"
#include <string.h>
#include <stdexcept>

std::string Set::code()
{
    return "M19";
}

std::string Set::name()
{
    return "Core Set 2019";
}

struct tm Set::release()
{
    struct tm ret;
    memset(&ret, 0, sizeof(ret));
}

std::string Card::name()
{
    return "Goblin Motivator";
}

std::string Card::oracle()
{
    return "{t}: Target creature gains haste until end of turn.";
}

std::string Card::types()
{
    return "Creature - Goblin";
}

double Card::cmc()
{
    return 1;
}

double Card::power()
{
    return 1;
}

double Card::toughness()
{
    return 1;
}

std::list<Set> Card::sets()
{
    std::list<Set> ret;
    return ret;
}

SearchResult::SearchResult(mse_t *mse, mse_search_result_t res)
{
    this->mse = mse;
    this->res = res;
}

SearchResult::~SearchResult()
{
    mse_free_search_results(&res);
}

SearchResult *MTGSearchEngine::search(std::string query)
{
    mse_search_result_t result;
    if (!mse_search(&this->mse, &result, query.c_str())) {
        throw std::runtime_error("Search error");
    }
    return new SearchResult(&this->mse, result);
}

MTGSearchEngine::MTGSearchEngine()
{
    if (!mse_init(&this->mse)) {
        throw std::runtime_error("Cannot init magic search engine");
    }
}

MTGSearchEngine::~MTGSearchEngine()
{
    lprintf(LOG_ERROR, "Freeing this object is prohibited.\n");
    mse_free(&this->mse);
    throw std::runtime_error("This object should never be freed outside of tests");
}
