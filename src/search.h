#pragma once
#include <mse/mse.h>
#include <string>
#include <list>
#include <time.h>

class Set
{
public:
    Set(mse_t *mse, mse_set_code_t set_code);
    std::string code();
    std::string name();
    struct tm release();
private:
    mse_set_t *set;
};

class Card
{
public:
    Card(mse_t *mse, mse_card_t *card);
    std::string name();
    std::string oracle();
    std::string types();
    double cmc();
    double power();
    double toughness();
    std::list<Set> sets();
private:
    mse_card_t *card;
    mse_t *mse;
};

class SearchResult
{
public:
    SearchResult(mse_t *mse, mse_search_result_t res);
    ~SearchResult();
    size_t size();
    Card at(size_t i);
    void sort(mse_search_sort_type_t sort_type);
private:
    mse_t *mse;
    mse_search_result_t res;
};

class MTGSearchEngine
{
public:
    static MTGSearchEngine &get_instance()
    {
        static MTGSearchEngine instance = MTGSearchEngine();
        return instance;
    }
    MTGSearchEngine(MTGSearchEngine &other) = delete;
    void operator=(const MTGSearchEngine &) = delete;
    SearchResult *search(std::string query);
private:
    ~MTGSearchEngine();
    MTGSearchEngine();
    mse_t mse;
};
