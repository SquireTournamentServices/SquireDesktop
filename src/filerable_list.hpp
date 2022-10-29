#pragma once
#include <vector>
#include <string>
#include <algorithm>

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
    FilteredList(std::vector<T> base, int (*current_sort)(const T &a, const T &b));
    void setBase(std::vector<T> base);
    void filter(std::string query);
    void sort(int (*current_sort)(const T &a, const T &b));
    void insert(T element);
    std::vector<T> getFiltered() const;
    size_t osize() const;
    size_t size() const;
    T at(size_t i) const;
    void setAscending(bool asc);
private:
    std::vector<T> original;
    std::vector<T> filtered;
    std::string lastSearch;
    bool ascending;
    int (*current_sort)(const T &a, const T &b); // Comparison function
    void insertToVect(std::vector<T> &t, T element);
    void removeFromVect(T element);
    int getIndex(T element);
    int __no_err_binary_search(std::vector<T> &vec, T &item, int s1, int s2);
    int no_err_binary_search(std::vector<T> &vec, T &item);
};

template <class T>
FilteredList<T>::FilteredList()
{
    // Do nothing
}

template <class T>
FilteredList<T>::FilteredList(std::vector<T> base, int (*current_sort)(const T &a, const T &b))
{
    this->ascending = true;
    this->original = base;
    this->current_sort = current_sort;
    std::stable_sort(this->original.begin(), this->original.end(), this->current_sort);

    this->filter("");
}

template <class T>
void FilteredList<T>::setBase(std::vector<T> base)
{
    this->original = base;
    std::stable_sort(this->original.begin(), this->original.end(), this->current_sort);
    this->filter(this->lastSearch);
}

template <class T>
void FilteredList<T>::filter(std::string query)
{
    this->lastSearch = query;

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
}

template <class T>
void FilteredList<T>::sort(int (*current_sort)(const T &a, const T &b))
{
    this->current_sort = current_sort;
    std::stable_sort(this->original.begin(), this->original.end(), this->current_sort);
    std::stable_sort(this->filtered.begin(), this->filtered.end(), this->current_sort);
}

template <class T>
int FilteredList<T>::__no_err_binary_search(std::vector<T> &vec, T &item, int s1, int s2)
{
    if (s1 > s2) {
        return s1;
    }

    int middle = (s1 + s2) / 2;

    int r = this->current_sort(item, vec.at(middle));
    if (r == 0) {
        return middle;
    }

    if (r < 0) {
        return __no_err_binary_search(vec, item, middle + 1, s2);
    } else {
        return __no_err_binary_search(vec, item, s1, middle - 1);
    }
}

template <class T>
int FilteredList<T>::no_err_binary_search(std::vector<T> &vec, T &item)
{
    return __no_err_binary_search(vec, item, 0, vec.size() - 1);
}

template <class T>
void FilteredList<T>::insertToVect(std::vector<T> &vec, T element)
{
    int index = this->no_err_binary_search(vec, element);
    vec.insert(vec.begin() + index, element);
}

template <class T>
void FilteredList<T>::insert(T element)
{
    this->insertToVect(this->original, element);
    if (this->lastSearch == "" || element.matches(this->lastSearch)) {
        this->insertToVect(this->filtered, element);
    }
}

template <class T>
size_t FilteredList<T>::osize() const
{
    return this->original.size();
}

template <class T>
size_t FilteredList<T>::size() const
{
    return this->filtered.size();
}

template <class T>
std::vector<T> FilteredList<T>::getFiltered() const
{
    if (!this->ascending) {
        std::vector<T> ret = std::vector<T>();
        for (long i = this->filtered.size() - 1; i >= 0; i--) {
            ret.push_back(this->filtered[i]);
        }
        return ret;
    }
    return this->filtered;
}

template <class T>
T FilteredList<T>::at(size_t i) const
{
    return this->filtered[i];
}

template <class T>
void FilteredList<T>::removeFromVect(T element)
{
    int index_filtered = this->getIndex(element);
    if (index_filtered != -1) {
        this->Filtered.remove(index_filtered);
    }

    // Binary search in main list
    int i = binary_search(this->original, element);
    if (i != -1) {
        this->original.remove(i);
    }
}

template <class T>
void FilteredList<T>::setAscending(bool asc)
{
    this->ascending = asc;
}
