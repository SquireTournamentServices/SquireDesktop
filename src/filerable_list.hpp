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
    size_t size() const;
private:
    std::vector<T> original;
    std::vector<T> filtered;
    std::string lastSearch;
    int (*current_sort)(const T &a, const T &b); // Comparison function
    void insertToVect(std::vector<T> &t, T element);
    void removeFromVect(T element);
    int getIndex(T element);
};

template <class T>
FilteredList<T>::FilteredList()
{
    // Do nothing
}

template <class T>
FilteredList<T>::FilteredList(std::vector<T> base, int (*current_sort)(const T &a, const T &b))
{
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
void FilteredList<T>::insertToVect(std::vector<T> &vec, T element)
{
    if (vec.size() == 0) {
        vec.push_back(element);
    } else {
        int min = 0;
        int max = vec.size();
        int i = vec.size() / 2;
        while (min < max) { // If equal then insert in place right?
            i = (max - min) / 2;

            // Greater than or equal
            if (this->current_sort(vec.at(i), element)) {
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
void FilteredList<T>::insert(T element)
{
    this->insert_to_vect(this->original, element);
    if (this->lastSearch == "" || element.matches(this->lastSearch)) {
        this->insert_to_vect(this->filtered, element);
    }
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

template <class T>
static int __binary_search(std::vector<T> &vec, T &item, int s1, int s2) {
    if (s1 > s2) {
        return -1;
    }

    int middle = (s1 + s2) / 2;

    if (item == vec.at(middle)) {
        return middle;
    }

    if (item > vec.at(middle)) {
        return __binary_search(vec, item, middle + 1, s2);
    } else {
        return __binary_search(vec, item, s1, middle - 1);
    }
}

template <class T>
int binary_search(std::vector<T> &vec, T &item)
{
    return __binary_search(vec, item, 0, vec.size() - 1);
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
int FilteredList<T>::getIndex(T element)
{
    return binary_search(this->original, element);
}

