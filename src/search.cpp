#include "./search.h"
#include <string.h>

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
