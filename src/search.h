#pragma once
#include <mse/mse.h>
#include <string>
#include <list>
#include <time.h>

class Set
{
public:
    std::string code();
    std::string name();
    struct tm release();
private:
    mse_set_code_t set_code;
};

class Card
{
public:
    std::string name();
    std::string oracle();
    std::string types();
    double cmc();
    double power();
    double toughness();
    std::list<Set> sets();
private:
    mse_uuid_t card_id;
};

class SearchResult
{

};

class MTGSearchEngine
{
public:
    ~MTGSearchEngine();
    static MTGSearchEngine *get_instance();
    MTGSearchEngine(MTGSearchEngine &other) = delete;
    void operator=(const MTGSearchEngine &) = delete;
    SearchResult search(std::string query);
private:
    MTGSearchEngine();
    mse_t mse;
};
