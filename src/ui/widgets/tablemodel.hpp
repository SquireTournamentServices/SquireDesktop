#pragma once
#include <stddef.h>
#include <vector>
#include <QModelIndex>
#include <QAbstractTableModel>

/*
   Override .cols() to return the amount of columns
   Override QAbstractTableModel as usual
   All sorting and, searching is done by searchsorttable.h|c(pp)?
 */
template <class T>
class TableModel : public QAbstractTableModel
{
public:
    TableModel(std::vector<T> data)
    {
        this->data = data;
    }
    ~TableModel() {}
    virtual size_t cols()
    {
        return -1;
    }
public slots:
    void rerender()
    {
        QModelIndex topLeft = this->index(0, 0);
        QModelIndex bottomRight = this->index(this->items.size() - 1, this->cols() - 1);
        emit this->dataChanged(topLeft, bottomRight);
    }
    void setData(std::vector<T> data)
    {
        this->data = data;
        this->rerender();
    }
private:
    std::vector<T> data;
};
