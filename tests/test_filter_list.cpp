#include "./test_filter_list.h"
#include "../src/filerable_list.hpp"
#include "../testing_h/testing.h"
#include <string.h>

class Foo
{
public:
    Foo(std::string a)
    {
        this->a = a;
    }
    bool matches(std::string query)
    {
        return query == this->a;
    }
    std::string a;
};

#define MATCH_STR "asdfasdfasdfasdf"

bool foo_sort(const Foo &fooa, const Foo &foob)
{
    return strcmp(fooa.a.c_str(), foob.a.c_str());
}

static int test_init()
{
    Foo a = Foo(MATCH_STR);
    Foo b = Foo(MATCH_STR MATCH_STR);

    std::vector<Foo> list;

    list.push_back(a);
    list.push_back(b);

    FilteredList<Foo> flist = FilteredList(list, foo_sort);
}

SUB_TEST(filter_list_tests,
{&test_init, "Test init"}
        )


