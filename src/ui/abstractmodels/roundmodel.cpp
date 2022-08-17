#include "./roundmodel.h"
#define COLS 3

RoundModel::RoundModel(std::vector<Round> rounds) :
    TableModel<Round>(rounds)
{

}

RoundModel::~RoundModel()
{

}

int RoundModel::columnCount(const QModelIndex &parent) const
{
    return COLS;
}

QVariant RoundModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    if (orientation == Qt::Orientation::Vertical) {
        return QVariant(section + 1);
    }

    switch (section) {
    case 0:
        return QVariant(tr("Match Number"));
    case 1:
        return QVariant(tr("Time Left"));
    case 2:
        return QVariant(tr("Players"));
    }
    return QVariant();
}

QVariant RoundModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole) {
        return QVariant();
    }

    if (index.row() >= (int) this->mdldata.size() || index.column() >= COLS) {
        return QVariant();
    }

    Round round = this->mdldata[index.row()];
    switch (index.column()) {
    case 0:
        return QVariant(round.matchNumber());
    case 1:
        long timeLeft = round.timeLeft();
        int seconds = timeLeft % 60;
        int minutes = ((timeLeft / 60) % 60);
        int hours = timeLeft / (60 * 60);

        QString str = "";
        if (hours < 10) {
            str += "0";
        }
        str += hours;
        str += ":";

        if (minutes < 10) {
            str += "0";
        }
        str += minutes;
        str += ":";

        if (seconds < 10) {
            str += "0";
        }
        str += seconds;

        return str;
    }
    return QVariant();
}

