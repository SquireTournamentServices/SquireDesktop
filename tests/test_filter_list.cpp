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
    bool operator==(Foo &other)
    {
        return this->a == other.a;
    }
    std::string a;
};

#define MATCH_STR "asdfasdfasdfasdf"

static int foo_sort(const Foo &fooa, const Foo &foob)
{
    return strcmp(fooa.a.c_str(), foob.a.c_str());
}

static int foo_sort_inv(const Foo &fooa, const Foo &foob)
{
    return -strcmp(fooa.a.c_str(), foob.a.c_str());
}

static int test_init()
{
    Foo a = Foo(MATCH_STR);
    Foo b = Foo(MATCH_STR MATCH_STR);

    std::vector<Foo> list;

    list.push_back(a);
    list.push_back(b);

    FilteredList<Foo> flist = FilteredList(list, foo_sort);
    ASSERT(flist.getFiltered().size() == 2);
    ASSERT(flist.size() == 2);
    for (int i = 0; i < 2; i++) {
        ASSERT(flist.at(i) == flist.getFiltered()[i]);
    }

    return 1;
}

static int test_search()
{
    Foo a = Foo(MATCH_STR);
    Foo b = Foo(MATCH_STR MATCH_STR);

    std::vector<Foo> list;

    list.push_back(a);
    list.push_back(b);

    FilteredList<Foo> flist = FilteredList(list, foo_sort);
    ASSERT(a.matches(MATCH_STR));
    ASSERT(!b.matches(MATCH_STR));

    flist.filter(MATCH_STR);
    ASSERT(flist.getFiltered().size() == 1);
    ASSERT(flist.getFiltered()[0] == a);

    flist.filter("");
    ASSERT(flist.getFiltered().size() == 2);
    ASSERT(flist.getFiltered()[0] == b);
    ASSERT(flist.getFiltered()[1] == a);

    return 1;
}

static int test_sort()
{
    Foo a = Foo(MATCH_STR);
    Foo b = Foo(MATCH_STR MATCH_STR);

    std::vector<Foo> list;
    list.push_back(a);
    list.push_back(b);

    FilteredList<Foo> flist = FilteredList(list, foo_sort);

    // Sanity check sorts
    std::stable_sort(list.begin(), list.end(), foo_sort);
    ASSERT(list[0] == b);
    ASSERT(list[1] == a);

    std::stable_sort(list.begin(), list.end(), foo_sort_inv);
    ASSERT(list[0] == a);
    ASSERT(list[1] == b);

    // Assert output of flist
    ASSERT(flist.getFiltered().size() == 2);
    ASSERT(flist.getFiltered()[0] == b);
    ASSERT(flist.getFiltered()[1] == a);

    // Test changing the sort
    flist.sort(foo_sort_inv);
    ASSERT(flist.size() == 2);
    ASSERT(flist.getFiltered().size() == 2);
    ASSERT(flist.getFiltered()[0] == a);
    ASSERT(flist.getFiltered()[1] == b);

    return 1;
}

static int test_set_base()
{
    Foo a = Foo(MATCH_STR);
    Foo b = Foo(MATCH_STR MATCH_STR);
    Foo c = Foo(MATCH_STR MATCH_STR MATCH_STR);

    std::vector<Foo> list;
    list.push_back(a);
    FilteredList<Foo> flist = FilteredList(list, foo_sort);
    ASSERT(flist.osize() == 1);
    ASSERT(flist.getFiltered().size() == 1);
    ASSERT(flist.getFiltered()[0] == a);

    // Tests sort is kept
    list = std::vector<Foo>();
    list.push_back(c);
    list.push_back(a);
    list.push_back(b);

    flist.setBase(list);

    std::stable_sort(list.begin(), list.end(), foo_sort);
    ASSERT(flist.size() == 3);
    ASSERT(flist.osize() == 3);
    ASSERT(flist.getFiltered().size() == 3);
    ASSERT(flist.getFiltered()[0] == list[0]);
    ASSERT(flist.getFiltered()[1] == list[1]);
    ASSERT(flist.getFiltered()[2] == list[2]);

    // Tests filter is kept
    flist.filter(MATCH_STR);
    ASSERT(flist.osize() == 3);
    ASSERT(flist.getFiltered().size() == 1);

    flist.setBase(list);
    ASSERT(flist.getFiltered().size() == 1);
    ASSERT(flist.size() == 1);
    ASSERT(flist.getFiltered()[0] == a);

    return 1;
}

static int test_insert()
{
    Foo a = Foo(MATCH_STR);
    Foo c = Foo(MATCH_STR MATCH_STR MATCH_STR);

    std::vector<Foo> list;
    list.push_back(a);
    FilteredList<Foo> flist = FilteredList(list, foo_sort);
    ASSERT(flist.getFiltered().size() == 1);
    ASSERT(flist.getFiltered()[0] == a);

    // Tests sort is kept - 1
    list.push_back(c);

    flist.insert(c);
    ASSERT(flist.osize() == 2);
    ASSERT(flist.size() == 2);

    // Check insert order was correct
    std::stable_sort(list.begin(), list.end(), foo_sort);
    ASSERT(flist.getFiltered()[0] == list[0]);
    ASSERT(flist.getFiltered()[1] == list[1]);
    return 1;
}

static int test_insert_2()
{
    Foo a = Foo(MATCH_STR);
    Foo b = Foo(MATCH_STR MATCH_STR);
    Foo c = Foo(MATCH_STR MATCH_STR MATCH_STR);

    std::vector<Foo> list;
    list.push_back(a);
    FilteredList<Foo> flist = FilteredList(list, foo_sort);
    ASSERT(flist.size() == 1);
    ASSERT(flist.osize() == 1);

    // Tests sort is kept - 2
    list.push_back(b);
    list.push_back(c);

    flist.insert(c);
    ASSERT(flist.size() == 2);

    flist.insert(b);
    ASSERT(flist.size() == 3);

    // Check insert order was correct
    std::stable_sort(list.begin(), list.end(), foo_sort);
    ASSERT(flist.getFiltered()[0] == list[0]);
    ASSERT(flist.getFiltered()[1] == list[1]);
    ASSERT(flist.getFiltered()[2] == list[2]);
    return 1;
}

static int test_insert_3()
{
    Foo a = Foo(MATCH_STR);

    std::vector<Foo> list;
    FilteredList<Foo> flist = FilteredList(list, foo_sort);
    ASSERT(flist.size() == 0);

    flist.insert(a);
    ASSERT(flist.size() == 1);
    ASSERT(flist.at(0) == a);
    return 1;
}


static int test_insert_4()
{
    Foo a = Foo(MATCH_STR);

    std::vector<Foo> list;
    FilteredList<Foo> flist = FilteredList(list, foo_sort);
    ASSERT(flist.size() == 0);

    for (int i = 0; i < 1000; i++) {
        flist.insert(a);
        ASSERT(flist.osize() == i + 1);
        ASSERT(flist.size() == i + 1);
    }
    return 1;
}

SUB_TEST(filter_list_tests,
{&test_init, "Test init"},
{&test_search, "Test search"},
{&test_sort, "Test sort"},
{&test_set_base, "Test set base"},
{&test_insert, "Test insert"},
{&test_insert_2, " Test insert 2"},
{&test_insert_3, "Test insert 3"},
{&test_insert_4, "Test insert 4"}
        )


