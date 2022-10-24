#pragma once
#include <QVariant>
#include "../../model/player.h"
#include "../widgets/tablemodel.hpp"

class PlayerScoreModel : public TableModel<PlayerScore>
{
public:
    PlayerScoreModel(std::vector<PlayerScore> players);
    ~PlayerScoreModel();
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
};

