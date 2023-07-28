#include "./search.h"
#include "../testing_h/testing.h"
#include <string.h>
#include <stdexcept>

Set::Set(mse_set_code_t set_code)
{
    memcpy(this->set_code, set_code, sizeof(set_code));
}

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
    return ret;
}

Card::Card(mse_card_t *card)
{
    this->card = card;
}

std::string Card::name()
{
    return std::string(this->card->name);
}

std::string Card::oracle()
{
    return std::string(this->card->oracle_text);
}

std::string Card::types()
{
    std::string ret = "";
    for (size_t i = 0; i < this->card->types_count; i++, ret += " ") {
        ret += std::string(this->card->types[i]);
    }
    return ret;
}

double Card::cmc()
{
    return this->card->cmc;
}

double Card::power()
{
    return this->card->power;
}

double Card::toughness()
{
    return this->card->toughness;
}

std::list<Set> Card::sets()
{
    std::list<Set> ret;
    for (size_t i = 0; i < this->card->set_codes_count; i++) {
        ret.push_back(this->card->set_codes[i]);
    }
    return ret;
}

size_t SearchResult::size()
{
    return this->res.cards_length;
}

Card SearchResult::at(size_t i)
{
    if (i >= this->res.cards_length) {
        throw std::runtime_error("Invalid index");
    }

    return Card(this->res.cards[i]);
}

void SearchResult::sort(mse_search_sort_type_t sort_type)
{
    mse_sort_search_results(&this->res, sort_type);
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
    mse_free(&this->mse);
}
