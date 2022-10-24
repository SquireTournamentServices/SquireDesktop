#include "./playerscoremodel.h"
// Name, match points, game points, mwp, gwp, opp_mwp, opp_gwp
#define COLS 7

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
    case 1:
        return QVariant(tr("Match Points"));
    case 2:
        return QVariant(tr("Game Points"));
    case 3:
        return QVariant(tr("Match Win %"));
    case 4:
        return QVariant(tr("Game Win %"));
    case 5:
        return QVariant(tr("Opp Match Win %"));
    case 6:
        return QVariant(tr("Opp Game Win %"));
    }
    return QVariant();
}

#define __DECIMAL_POINTS 1
#define DECIMAL_POINTS pow(10, __DECIMAL_POINTS)

static QVariant formatPercentage(double p)
{
    double perc = floor(p * 100 * DECIMAL_POINTS) / DECIMAL_POINTS;
    QString ret = QString::number(perc) + tr("%");
    return QVariant(ret);
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
    case 1:
        return QVariant(p.score().match_points);
    case 2:
        return QVariant(p.score().game_points);
    case 3:
        return formatPercentage(p.score().mwp);
    case 4:
        return formatPercentage(p.score().gwp);
    case 5:
        return formatPercentage(p.score().opp_mwp);
    case 6:
        return formatPercentage(p.score().opp_gwp);
    }
    return QVariant();
}

