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
