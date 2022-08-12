#pragma once
#include <QWidget>
#include <QVBoxLayout>
#include <QCheckBox>
#include <vector>
#include <string>
#include "./tablemodel.hpp"
#include "../../filerable_list.hpp"

namespace Ui
{
class SearchSortTableWidget;
}

class sstw_qobject : public QWidget
{
    Q_OBJECT
public:
    explicit sstw_qobject(Ui::SearchSortTableWidget *ui, QWidget *parent = nullptr);
    ~sstw_qobject();
public slots:
    void filter(QString query);
protected:
    void changeEvent(QEvent *e);
    bool isBoxSelected(int i);
    void addBox(std::string boxName);
private:
    std::vector<QCheckBox *> addFilterBoxes;
    Ui::SearchSortTableWidget *ui;
    QVBoxLayout *layout;
};

/*
   T_MDL must be a subclass of TableModel
   T_DATA must be used in T_MDL
 */
template <class T_MDL, class T_DATA>
class SearchSortTableWidget : public sstw_qobject
{
public:
    explicit SearchSortTableWidget(std::vector<T_DATA> data, QWidget *parent = nullptr);
    ~SearchSortTableWidget();
    void setData();
    void addDatum(T_DATA datum);
    //void removeDatum(T_DATA datum); //TODO
    void addSortAlg(int (*sort_alg)(const T_DATA &a, const T_DATA &b));
    void addAdditionalFilter(std::string boxName, bool(*matches)(T_DATA a));
    void filter(QString query);
private:
    std::vector<bool (*)(T_DATA a)> additionalFilters;
    std::vector<int (*)(const T_DATA &a, const T_DATA &b)> sortAls;
    std::vector<T_DATA> data;
    FilteredList<T_DATA> flist;
    TableModel<T_DATA> tableModel; // type = T_MDL at init
    Ui::SearchSortTableWidget *ui;

    void filterList();
    void sortList(int colIndex);
};

/*
   // Sample class for T_MDL
#pragma once
#include <QAbstractTableModel>
#include <QVariant>
#include "../model/inventoryitem.h"
#include "filteredlist.h"

// name, amount
#define INV_ITEM_TABLE_COLUMNS 2

class InventoryItemTable : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit InventoryItemTable(std::list<InventoryItem> items, QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    InventoryItem getItem(int i) const;
    void setItems(std::list<InventoryItem> items);
    void filter(std::string filter);
private:
    void updateTable();
    FilteredList<InventoryItem> items;
};
  */
