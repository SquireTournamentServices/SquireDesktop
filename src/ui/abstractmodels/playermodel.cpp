#include "./playermodel.h"
#define COLS 3

PlayerModel::PlayerModel(std::vector<Player> players) :
    TableModel<Player>(players)
{

}

PlayerModel::~PlayerModel()
{

}

int PlayerModel::columnCount(const QModelIndex &parent) const
{
    return COLS;
}

QVariant PlayerModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    if (orientation == Qt::Orientation::Vertical) {
        return QVariant(section + 1);
    }

    switch (section) {
    case 0:
        return QVariant(tr("Status"));
    case 1:
        return QVariant(tr("Name"));
    case 2:
        return QVariant(tr("Game Name"));
    }
    return QVariant();
}

QVariant PlayerModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole) {
        return QVariant();
    }

    if (index.row() >= (int) this->mdldata.size() || index.column() >= COLS) {
        return QVariant();
    }

    Player player = this->mdldata[index.row()];
    switch (index.column()) {
    case 0:
        return QVariant(QString::fromStdString(player.statusAsStr()));
    case 1:
        return QVariant(QString::fromStdString(player.name()));
    case 2:
        return QVariant(QString::fromStdString(player.gameName()));
    }
    return QVariant();
}

