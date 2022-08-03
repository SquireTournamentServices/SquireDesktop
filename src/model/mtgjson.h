#pragma once
#include <stddef.h>
#include <vector>

class MTGJson {
  public:
    MTGJson(const char *raw_data);
    ~MTGJson();
};

MTGJson initial_mtg_json();

