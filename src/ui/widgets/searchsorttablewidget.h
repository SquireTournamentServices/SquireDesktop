#pragma once
#include <QWidget>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QItemSelectionModel>
#include <vector>
#include <string>
#include "./tablemodel.hpp"
#include "../../../testing_h/logger.h"
#include "../../filerable_list.hpp"
#include "./ui_searchsorttablewidget.h"

class sstw_qobject : public QWidget
{
    Q_OBJECT
public:
    explicit sstw_qobject(Ui::SearchSortTableWidget *ui, QWidget *parent = nullptr);
    ~sstw_qobject();
    void finishSstwSetup(Ui::SearchSortTableWidget *ui);
public slots:
    virtual void onFilterChange(QString query);
    virtual void addFilter();
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
    void setData(std::vector<T_DATA> data);
    void addDatum(T_DATA datum);
    //void removeDatum(T_DATA datum); //TODO
    void addSortAlg(int (*sort_alg)(const T_DATA &a, const T_DATA &b));
    void addAdditionalFilter(std::string boxName, bool(*matches)(T_DATA a));
    void onFilterChange(QString query) override;
    void addFilter() override;
private:
    std::vector<bool (*)(T_DATA a)> additionalFilters;
    std::vector<int (*)(const T_DATA &a, const T_DATA &b)> sortAls;
    std::vector<T_DATA> data;
    QItemSelectionModel *itemMdl;
    FilteredList<T_DATA> flist;
    TableModel<T_DATA> *tableModel; // type = T_MDL at init
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

template <class T_MDL, class T_DATA>
SearchSortTableWidget<T_MDL, T_DATA>::SearchSortTableWidget(std::vector<T_DATA> data, QWidget *parent) :
    ui(new Ui::SearchSortTableWidget),
    sstw_qobject(ui, parent)
{
    ui->setupUi(this);
    this->finishSstwSetup(ui);
    this->data = data;
    this->flist = FilteredList<T_DATA>(this->data, (
                                           int (*)(const T_DATA &, const T_DATA &)) T_DATA().getDefaultSort());
    this->tableModel = new T_MDL(this->flist.getFiltered());
    this->itemMdl = new QItemSelectionModel(this->tableModel);

    ui->table->setModel(this->tableModel);
}

template <class T_MDL, class T_DATA>
SearchSortTableWidget<T_MDL, T_DATA>::~SearchSortTableWidget()
{
    delete ui;
    delete this->tableModel;
    delete this->itemMdl;
}

template <class T_MDL, class T_DATA>
void SearchSortTableWidget<T_MDL, T_DATA>::setData(std::vector<T_DATA> data)
{
    this->data = data;
    this->filterList();
}

template <class T_MDL, class T_DATA>
void SearchSortTableWidget<T_MDL, T_DATA>::addDatum(T_DATA datum)
{
    this->data.push_back(datum);
    this->filterList();
}

//void removeDatum(T_DATA datum); //TODO

template <class T_MDL, class T_DATA>
void SearchSortTableWidget<T_MDL, T_DATA>::addSortAlg(int (*sort_alg)(const T_DATA &a, const T_DATA &b))
{
    this->sortAls.push_back(sort_alg);
}

template <class T_MDL, class T_DATA>
void SearchSortTableWidget<T_MDL, T_DATA>::addAdditionalFilter(std::string boxName, bool(*matches)(T_DATA a))
{
    this->addBox(boxName);
    this->additionalFilters.push_back(matches);
}

template <class T_MDL, class T_DATA>
void SearchSortTableWidget<T_MDL, T_DATA>::onFilterChange(QString query)
{
    this->flist.filter(query.toStdString());
    this->tableModel->setData(this->flist.getFiltered());
}

template <class T_MDL, class T_DATA>
void SearchSortTableWidget<T_MDL, T_DATA>::addFilter()
{
    this->filterList();
}

template <class T_MDL, class T_DATA>
void SearchSortTableWidget<T_MDL, T_DATA>::filterList()
{
    // Only add an item when it matches all filters
    std::vector<T_DATA> filtered;
    for (int j = 0; j < this->data.size(); j++) {
        bool add = true;
        for (int i = 0; i < this->additionalFilters.size(); i++) {
            if (this->isBoxSelected(i)) {
                add &= this->additionalFilters[i](this->data[j]);
            }
        }

        if (add) {
            filtered.push_back(this->data[j]);
        }
    }

    this->flist.setBase(filtered);
    this->tableModel->setData(this->flist.getFiltered());
}

template <class T_MDL, class T_DATA>
void SearchSortTableWidget<T_MDL, T_DATA>::sortList(int colIndex)
{
    if (colIndex < this->sortAls.size()) {
        this->flist.sort(this->sortAls[colIndex]);
        this->tableModel->setData(this->flist.getFiltered());
    } else {
        lprintf(LOG_WARNING, "Index %d has no sorting algorithm\n", colIndex);
    }
}
