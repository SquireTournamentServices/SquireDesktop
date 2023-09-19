#include "./searchmodel.h"
#include "../../../testing_h/testing.h"
#include <stdexcept>

#define COLS 6

SearchModel::SearchModel()
{
    this->mse = &MTGSearchEngine::get_instance();
    this->result = nullptr;
    this->ascending = true;
}

SearchModel::~SearchModel()
{
    if (this->result != nullptr) {
        delete this->result;
    }
}

int SearchModel::columnCount(const QModelIndex &parent) const
{
    return COLS;
}

int SearchModel::rowCount(const QModelIndex &parent) const
{
    if (this->result != nullptr) {
        return this->result->size();
    }
    return 0;
}

QVariant SearchModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    switch(section) {
    case 0:
        return QVariant("Name");
    case 1:
        return QVariant("CMC");
    case 2:
        return QVariant("Colours");
    case 3:
        return QVariant("Power");
    case 4:
        return QVariant("Toughness");
    case 5:
        return QVariant("Types");
    }
    return QVariant();

}

QVariant SearchModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole) {
        return QVariant();
    }

    if (index.row() >= this->rowCount() || index.column() >= this->columnCount()) {
        return QVariant();
    }

    Card card = this->result->at(index.row());

    if (this->ascending) {
        card = this->result->at(this->result->size() - index.row() - 1);
    }
    switch (index.column()) {
    case 0:
        return QVariant(QString::fromStdString(card.name()));
    case 1:
        return QVariant(card.cmc());
    case 2:
        return QVariant("Colours TODO");
    case 3:
        return QVariant(card.power());
    case 4:
        return QVariant(card.toughness());
    case 5:
        return QVariant(QString::fromStdString(card.types()));
    }
    return QVariant();
}

void SearchModel::rerender()
{
    this->beginResetModel();
    QModelIndex topLeft = this->index(0, 0);
    QModelIndex bottomRight = this->index(this->rowCount() - 1, this->columnCount() - 1);
    this->dataChanged(topLeft, bottomRight);
    this->endResetModel();
}

void SearchModel::sort(int column, Qt::SortOrder order)
{
    mse_search_sort_type_t type;
    if (this->result != nullptr) {
        type = this->result->sortType();
    }

    switch(column) {
    case 0:
        type = MSE_SORT_CARD_NAME;
        break;
    case 1:
        type = MSE_SORT_CMC;
        break;
    case 2:
        type = MSE_SORT_POWER;
        break;
    case 3:
        type = MSE_SORT_TOUGHNESS;
        break;
    }

    this->ascending = order == Qt::AscendingOrder;
    if (this->result != nullptr) {
        this->result->sort(type);
    }
    this->rerender();
}

void SearchModel::set_query(std::string query)
{
    if (this->result != nullptr) {
        delete this->result;
    }
    this->result = nullptr;

    try {
        this->result = this->mse->search(query);
    } catch(std::exception &err) {
        lprintf(LOG_ERROR, "Cannot search for `%s` due to %s\n", query.c_str(), err.what());
        return;
    }

    this->rerender();
}
