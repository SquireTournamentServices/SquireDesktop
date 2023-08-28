#include "./search.h"
#include "../testing_h/testing.h"
#include <string.h>
#include <stdexcept>

static mse_set_t *__lookup_set(mse_avl_tree_node_t *node, mse_set_code_t set_code)
{
    if (node == NULL) {
        return NULL;
    }

    mse_set_t *payload = (mse_set_t *) node->payload;
    if (memcmp(payload->code, set_code, sizeof(mse_set_code_t)) == 0) {
        return payload;
    }

    mse_set_t *ret = NULL;
    if ((ret = __lookup_set(node->l, set_code))) {
        return ret;
    }
    if ((ret = __lookup_set(node->r, set_code))) {
        return ret;
    }
    return  NULL;
}

Set::Set(mse_t *mse, mse_set_code_t set_code)
{
    this->set = __lookup_set(mse->cards.set_tree, set_code);
    if (this->set == NULL) {
        throw std::runtime_error("Cannot find set");
    }
}

std::string Set::code()
{
    char buffer[sizeof(this->set->code)];
    memcpy(buffer, this->set->code, sizeof(buffer));
    return std::string(buffer);
}

std::string Set::name()
{
    return std::string(this->set->name);
}

struct tm Set::release()
{
    struct tm ret;
    memcpy(&ret, &this->set->release, sizeof(ret));
    return ret;
}

Card::Card(mse_t *mse, mse_card_t *card)
{
    this->mse = mse;
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
        ret.push_back(Set(this->mse, this->card->set_codes[i]));
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

    return Card(this->mse, this->res.cards[i]);
}

void SearchResult::sort(mse_search_sort_type_t sort_type)
{
    mse_sort_search_results(&this->res, sort_type);
    this->sort_type = sort_type;
}

mse_search_sort_type_t SearchResult::sortType()
{
    return this->sort_type;
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
