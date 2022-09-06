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
    void finishSstwSetup(Ui::SearchSortTableWidget *ui, tm_qobject *sortObject);
public slots:
    virtual void onFilterChange(QString query);
    virtual void addFilter();
    virtual void sortChanged(int column, bool ascending);
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
    void sortChanged(int column, bool ascending) override;
private:
    std::vector<bool (*)(T_DATA a)> additionalFilters;
    std::vector<int (*)(const T_DATA &a, const T_DATA &b)> sortAlgs;
    std::vector<T_DATA> data;
    QItemSelectionModel *itemMdl;
    FilteredList<T_DATA> flist;
    TableModel<T_DATA> *tableModel; // type = T_MDL at init
    Ui::SearchSortTableWidget *ui;

    void filterList();
};
// See ../abstractmodels/playermodel.* for an example of how to implement this

template <class T_MDL, class T_DATA>
SearchSortTableWidget<T_MDL, T_DATA>::SearchSortTableWidget(std::vector<T_DATA> data, QWidget *parent) :
    ui(new Ui::SearchSortTableWidget),
    sstw_qobject(ui, parent)
{
    ui->setupUi(this);
    this->data = data;
    this->sortAlgs = T_DATA().getDefaultAlgs();
    this->flist = FilteredList<T_DATA>(this->data, this->sortAlgs.size() == 0 ? NULL : this->sortAlgs[0]);
    this->tableModel = new T_MDL(this->flist.getFiltered());
    this->itemMdl = new QItemSelectionModel(this->tableModel);

    this->finishSstwSetup(ui, this->tableModel->getSortObject());
    ui->table->setModel(this->tableModel);
}

template <class T_MDL, class T_DATA>
SearchSortTableWidget<T_MDL, T_DATA>::~SearchSortTableWidget()
{
    delete ui;
    delete this->itemMdl;
    delete this->tableModel;
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
void SearchSortTableWidget<T_MDL, T_DATA>::sortChanged(int column, bool ascending)
{
    this->flist.setAscending(ascending);
    if (column < this->sortAlgs.size()) {
        this->flist.sort(this->sortAlgs[column]);
        this->tableModel->setData(this->flist.getFiltered());
    } else {
        lprintf(LOG_WARNING, "Index %d has no sorting algorithm\n", column);
    }
}
