#pragma once
#include <QVariant>
#include <QObject>
#include <QModelIndex>
#include <QAbstractTableModel>
#include <string>
#include "../../search.h"

class SearchModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    SearchModel();
    ~SearchModel();
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;
    void set_query(std::string query);
private:
    void rerender();
    MTGSearchEngine *mse;
    SearchResult *result;
    bool ascending;
};
