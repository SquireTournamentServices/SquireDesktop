#include "./search.h"
#include "../testing_h/testing.h"
#include <string.h>
#include <pthread.h>
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

MTGSearchEngine *MTGSearchEngine::get_instance()
{
    static MTGSearchEngine *instance = NULL;
    static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

    pthread_mutex_lock(&lock);
    if (instance == NULL) {
        lprintf(LOG_INFO, "Creating mse instance\n");
        instance = new MTGSearchEngine();
    }

    MTGSearchEngine *ret = instance;
    pthread_mutex_unlock(&lock);
    return ret;
}

SearchResult MTGSearchEngine::search(std::string query)
{
    return SearchResult();
}

MTGSearchEngine::MTGSearchEngine()
{
    if (!mse_init(&this->mse)) {
        throw std::runtime_error("Cannot init magic search engine");
    }
}

MTGSearchEngine::~MTGSearchEngine()
{
    mse_free(&this->mse);
}
