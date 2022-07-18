#pragma once
#include <QWidget>
#include "./widgets/labelimage.h"
#include "../config.h"

namespace Ui
{
class RecentTournamentWidget;
}

class RecentTournamentWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RecentTournamentWidget(recent_tournament_t t, QWidget *parent = nullptr);
    ~RecentTournamentWidget();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::RecentTournamentWidget *ui;
    recent_tournament_t t;
    LabelImage *img;
};

