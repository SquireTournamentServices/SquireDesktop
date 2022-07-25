#pragma once
#include <QString>
#include <QWidget>
#include "./labelimage.h"
#include "../../config.h"

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
signals:
    void openTournament(QString name);

protected:
    void changeEvent(QEvent *e);
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    Ui::RecentTournamentWidget *ui;
    recent_tournament_t t;
    LabelImage *img;
};

