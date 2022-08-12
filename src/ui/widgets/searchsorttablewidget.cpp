#include "./searchsorttablewidget.h"
#include "./ui_searchsorttablewidget.h"
#include "./tablemodel.hpp"
#include "../../../testing_h/logger.h"

sstw_qobject::sstw_qobject(Ui::SearchSortTableWidget *ui, QWidget *parent) :
    QWidget(parent)
{
    this->ui = ui;
    this->addFilterBoxes = std::vector<QCheckBox *>();

    this->layout = new QVBoxLayout(ui->cont);
    this->layout->setAlignment(Qt::AlignTop);

    connect(ui->searchbar, &QLineEdit::textChanged, this, &sstw_qobject::filter);
}

sstw_qobject::~sstw_qobject()
{
    for (QCheckBox *box : this->addFilterBoxes) {
        delete box;
    }
    delete this->layout;
}

void sstw_qobject::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

bool sstw_qobject::isBoxSelected(int i)
{
    if (i < this->addFilterBoxes.size()) {
        return this->addFilterBoxes[i]->isChecked();
    } else {
        return false;
    }
}

void sstw_qobject::addBox(std::string boxName)
{
    QCheckBox *box = new QCheckBox(QString::fromStdString(boxName));
    this->addFilterBoxes.push_back(box);
    this->layout->addWidget(box);
}

void sstw_qobject::filter(QString query)
{}

template <class T_MDL, class T_DATA>
SearchSortTableWidget<T_MDL, T_DATA>::SearchSortTableWidget(std::vector<T_DATA> data, QWidget *parent) :
    sstw_qobject(ui, parent),
    ui(new Ui::SearchSortTableWidget)
{
    ui->setupUi(this);
    this->data = data;
    this->flist = FilteredList<T_DATA>(data);
    this->tableModel = T_MDL(this->flist.getFiltered());
}

template <class T_MDL, class T_DATA>
SearchSortTableWidget<T_MDL, T_DATA>::~SearchSortTableWidget()
{
    delete ui;
}

template <class T_MDL, class T_DATA>
void SearchSortTableWidget<T_MDL, T_DATA>::setData()
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
void SearchSortTableWidget<T_MDL, T_DATA>::filter(QString query)
{
    this->flist.filter(query.toStdString());
    this->tableModel.setData(this->flist.getFiltered());
}

template <class T_MDL, class T_DATA>
void SearchSortTableWidget<T_MDL, T_DATA>::filterList()
{
    std::vector<T_DATA> filtered;
    for (int j = 0; j < this->data.size(); j++) {
        for (int i = 0; i < this->additionalFilters.size(); i++) {
            bool add = true;
            if (this->isBoxSelected(i)) {
                add = this->additionalFilters(this->data[j]);
            }

            if (add) {
                filtered.push_back(this->data[j]);
            }
        }
    }
    this->flist.setBase(filtered);
    this->tableModel.setData(this->flist.getFiltered());
}

template <class T_MDL, class T_DATA>
void SearchSortTableWidget<T_MDL, T_DATA>::sortList(int colIndex)
{
    if (colIndex < this->sortAls.size()) {
        this->flist.sort(this->sortAls[colIndex]);
        this->tableModel.setData(this->flist.getFiltered());
    } else {
        lprintf(LOG_WARNING, "Index %d has no sorting algorithm\n", colIndex);
    }
}
