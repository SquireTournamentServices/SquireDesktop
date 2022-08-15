#pragma once
#include <stddef.h>
#include <vector>
#include <QObject>
#include <QModelIndex>
#include <QAbstractTableModel>
#include "../../../testing_h/logger.h"

class tm_qobject: public QObject
{
    Q_OBJECT
signals:
    void sortChanged(int column, bool ascending);
public:
    tm_qobject() :
        QObject(nullptr)
    {

    }
    void onSortChanged(int column, bool ascending)
    {
        emit this->sortChanged(column, ascending);
    }
};

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
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    tm_qobject *getSortObject();
    void rerender();
    void setData(std::vector<T> data);
private:
    tm_qobject *sortIntermediate;
protected:
    std::vector<T> mdldata;
};

template <class T>
TableModel<T>::TableModel(std::vector<T> data)
{
    this->mdldata = data;
    this->rerender();
    this->sortIntermediate = new tm_qobject();
}

template <class T>
TableModel<T>::~TableModel()
{
    delete sortIntermediate;
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
void TableModel<T>::sort(int column, Qt::SortOrder order)
{
    this->sortIntermediate->onSortChanged(column, order == Qt::AscendingOrder);
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

template <class T>
tm_qobject *TableModel<T>::getSortObject()
{
    return this->sortIntermediate;
}
