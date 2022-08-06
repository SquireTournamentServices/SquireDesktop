#pragma once
#include <vector>
#include <string>
#include <algorithm>

#define NULL_FILTER NULL

/*
   A list of objects that can be filtered using the bool matches(std::string) method
   of class T. Example matches usage may just check if the input and a field are the
   same. If you want something to search without iterating each element then do not
   use this and, consider using TODO.
 */
template <class T>
class FilteredList
{
public:
    FilteredList(); // For deferred construction
    FilteredList(std::vector<T> base, bool (*current_sort)(T &a, T &b));
    void setBase(std::vecotr<T> base);
    void filter(std::string query);
    void sort(bool (*current_sort)(T &a, T &b));
    void insert(T element);
    std::vector<T> getFiltered() const;
    size_t size() const;
private:
    std::vector<T> original;
    std::vector<T> filtered;
    std::string lastSearch;
    bool (*current_sort)(T &a, T &b); // Comparison function
    void insert_to_vect(std::vector<T> &t, element T)
    {
    };

    template <class T>
    FilteredList<T>::FilteredList()
    {
        // Do nothing
    }

    template <class T>
    FilteredList<T>::FilteredList(std::vector<T> base, bool (*current_sort)(T &a, T &b))
    {
        this->current_sort = current_sort;
        std::stable_sort(base.begin(), base.end(), current_sort);

        this->original = base;
        this->filtered = base;
    }

    template <class T>
    void FilteredList<T>::filter(std::string query)
    {
        if (query == "") {
            this->filtered = this->original;
            return;
        }

        this->filtered = std::vector<T>();

        for (T item : this->original) {
            if (item.matches(query)) {
                this->filtered.push_back(item);
            }
        }

        this->lastSearch = query;
    }

    template <class T>
    void FilteredList::sort(bool (*current_sort)(T &a, T &b))
    {
        this->current_sort = current_sort;
        std::stable_sort(this->original.begin(), this->original.end(), current_sort);
        std::stable_sort(this->filtered.begin(), this->filtered.end(), current_sort);
    }

    template <class T>
    void FilteredList::insert_to_vect(std::vector<T> &vec, T element)
    {
        if (this->current_sort == NULL_FILTER || vec.size() == 0) {
            vec.push_back(element);
        } else {
            int min = 0;
            int max = vec.size();
            int i = vec.size() / 2;
            while (min < max) { // If equal then insert in place right?
                i = (max - min) / 2;

                // Greater than or equal
                if (this->current_sort(vec.at(i), T)) {
                    min = i + 1;
                } else {
                    max = i - 1;
                }
            }

            if (i < 0) {
                vec.push_front(element);
            } else if (i >= vec.size()) {
                vec.push_back(element);
            } else {
                vec.insert(i, element);
            }
        }
    }

    template <class T>
    void FilteredList::insert(T element)
    {
        this->insert_to_vect(this->original, element);
        if (element.matches(this->lastSearch)) {
            this->insert_to_vect(this->filtered, element);
        }
    }

    template <class T>
    void FilteredList<T>::setBase(std::vector<T> base)
    {
        std::stable_sort(base.begin(), base.end(), current_sort);
        this->original = base;
        this->filter(this->lastSearch);
    }

    template <class T>
    size_t FilteredList<T>::size() const
    {
        return this->filtered.size();
    }

    template <class T>
    std::vector<T> FilteredList<T>::getFiltered() const
    {
        return this->filtered;
    }

