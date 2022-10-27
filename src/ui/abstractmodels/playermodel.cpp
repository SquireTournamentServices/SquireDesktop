#include "./playermodel.h"
#include <QPixmap>
#include <QIcon>
#include "assets.h"
#define COLS 3

static QIcon *DROPPED_STATUS_ICON;
static QIcon *REGISTERED_STATUS_ICON;

void player_model_init_icons()
{
    QPixmap tmp;
    tmp.loadFromData(PLAYER_DROPPED_PNG, sizeof(PLAYER_DROPPED_PNG));
    DROPPED_STATUS_ICON = new QIcon(tmp);

    QPixmap tmp2;
    tmp2.loadFromData(PLAYER_REGISTERED_PNG, sizeof(PLAYER_DROPPED_PNG));
    REGISTERED_STATUS_ICON = new QIcon(tmp2);
}

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
        switch (player.status()) {
        case squire_core::sc_PlayerStatus::Dropped:
            return QVariant(*DROPPED_STATUS_ICON);
        case squire_core::sc_PlayerStatus::Registered:
            return QVariant(*REGISTERED_STATUS_ICON);
        }
        return QVariant("Error");
    case 1:
        return QVariant(QString::fromStdString(player.name()));
    case 2:
        return QVariant(QString::fromStdString(player.game_name()));
    }
    return QVariant();
}

