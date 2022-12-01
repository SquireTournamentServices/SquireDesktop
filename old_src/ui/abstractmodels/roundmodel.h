#pragma once
#include <QVariant>
#include "../../model/round.h"
#include "../widgets/tablemodel.hpp"

class RoundModel : public TableModel<Round>
{
    Q_OBJECT
public:
    RoundModel(std::vector<Round> rounds);
    ~RoundModel();
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
};

