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
        this->mdldata = data;
    }
    ~TableModel() {}
    virtual size_t cols()
    {
        return -1;
    }
    int rowCount(const QModelIndex &parent = QModelIndex());
    int columnCount(const QModelIndex &parent = QModelIndex());
    QVariant headerData(int section, Qt::Orientation orientation, int role);
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole);
    Qt::ItemFlags flags(const QModelIndex &index);
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
protected:
    std::vector<T> mdldata;
};
