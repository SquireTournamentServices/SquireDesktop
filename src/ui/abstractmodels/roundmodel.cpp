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

    // These must not be in my case statement
    long timeLeft;
    int seconds, minutes, hours;
    QString str;

    switch (index.column()) {
    case 0:
        return QVariant(round.match_number());
    case 1:
        timeLeft = round.time_left();
        seconds = timeLeft % 60;
        minutes = ((timeLeft / 60) % 60);
        hours = timeLeft / (60 * 60);

        str = "";
        if (hours < 10) {
            str += "0";
        }
        str += QString::number(hours, 10);
        str += ":";

        if (minutes < 10) {
            str += "0";
        }
        str += QString::number(minutes, 10);
        str += ":";

        if (seconds < 10) {
            str += "0";
        }
        str += QString::number(seconds, 10);

        return str;
    case 2:
        return QVariant(QString::fromStdString(round.players_as_str()));
    }
    return QVariant();
}

