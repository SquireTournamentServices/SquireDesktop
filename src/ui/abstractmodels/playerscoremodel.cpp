#include "./playerscoremodel.h"
#define COLS 1

PlayerScoreModel::PlayerScoreModel(std::vector<PlayerScore> playerScores) :
    TableModel<PlayerScore>(playerScores)
{

}

PlayerScoreModel::~PlayerScoreModel()
{

}

int PlayerScoreModel::columnCount(const QModelIndex &parent) const
{
    return COLS;
}

QVariant PlayerScoreModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    if (orientation == Qt::Orientation::Vertical) {
        return QVariant(section + 1);
    }

    switch (section) {
    case 0:
        return QVariant(tr("Player"));
    }
    return QVariant();
}

QVariant PlayerScoreModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole) {
        return QVariant();
    }

    if (index.row() >= (int) this->mdldata.size() || index.column() >= COLS) {
        return QVariant();
    }

    PlayerScore p = this->mdldata[index.row()];
    switch (index.column()) {
    case 0:
        return QVariant(QString::fromStdString(p.player().all_names()));
    }
    return QVariant();
}

