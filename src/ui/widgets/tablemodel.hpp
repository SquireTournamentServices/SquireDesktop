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
    TableModel()
    {

    }
    TableModel(std::vector<T> data)
    {
        this->mdldata = data;
    }
    ~TableModel() {}
    virtual size_t cols()
    {
        return -1;
    }
    int rowCount(const QModelIndex &parent = QModelIndex()) const override
    {
        return this->mdldata.size();
    }
    // This needs overriding
    int columnCount(const QModelIndex &parent = QModelIndex()) const override
    {
        return 0;
    }
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override
    {
        return QVariant();
    }
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override
    {
        return QVariant();
    }
    Qt::ItemFlags flags(const QModelIndex &index) const override
    {
        if(!index.isValid()) return Qt::ItemIsEnabled;
        return QAbstractTableModel::flags(index) | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }
    TableModel &operator=(const TableModel &t)
    {
        return *this;
    }
public slots:
    void rerender()
    {
        QModelIndex topLeft = this->index(0, 0);
        QModelIndex bottomRight = this->index(this->rowCount() - 1, this->cols() - 1);
        emit this->dataChanged(topLeft, bottomRight);
    }
    void setData(std::vector<T> data)
    {
        this->mdldata = data;
        this->rerender();
    }
protected:
    std::vector<T> mdldata;
};
