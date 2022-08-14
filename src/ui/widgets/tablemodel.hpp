#pragma once
#include <stddef.h>
#include <vector>
#include <QModelIndex>
#include <QAbstractTableModel>
#include "../../../testing_h/logger.h"

/*
   Override .cols() to return the amount of columns
   Override QAbstractTableModel as usual
   All sorting and, searching is done by searchsorttable.h|c(pp)?
 */
template <class T>
class TableModel : public QAbstractTableModel
{
public:
    TableModel(std::vector<T> data);
    ~TableModel();
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
public slots:
    void rerender();
    void setData(std::vector<T> data);
protected:
    std::vector<T> mdldata;
private:
};

template <class T>
TableModel<T>::TableModel(std::vector<T> data)
{
    this->mdldata = data;
    this->rerender();
}

template <class T>
TableModel<T>::~TableModel()
{

}

template <class T>
void TableModel<T>::rerender()
{
    this->beginResetModel();
    QModelIndex topLeft = this->index(0, 0);
    QModelIndex bottomRight = this->index(this->rowCount() - 1, this->columnCount() - 1);
    this->dataChanged(topLeft, bottomRight);
    this->endResetModel();
}

template <class T>
void TableModel<T>::setData(std::vector<T> data)
{
    this->mdldata = data;
    this->rerender();
}

template <class T>
int TableModel<T>::rowCount(const QModelIndex &parent) const
{
    return this->mdldata.size();
}

template <class T>
int TableModel<T>::columnCount(const QModelIndex &parent) const
{
    return 0;
}
