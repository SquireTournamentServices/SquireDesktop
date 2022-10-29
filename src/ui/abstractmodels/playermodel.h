#pragma once
#include <QVariant>
#include "../../model/player.h"
#include "../widgets/tablemodel.hpp"

class PlayerModel : public TableModel<Player>
{
    Q_OBJECT
public:
    PlayerModel(std::vector<Player> players);
    ~PlayerModel();
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
};

void player_model_init_icons();
