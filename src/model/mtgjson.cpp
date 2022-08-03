#include "mtgjson_initial.h"
#include "./mtgjson.h"

MTGJson::MTGJson(const char *raw_data)
{

}

MTGJson::~MTGJson()
{

}

MTGJson initial_mtg_json()
{
    return MTGJson(initial_json);
}

