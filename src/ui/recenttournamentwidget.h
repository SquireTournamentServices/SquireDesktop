#ifndef RECENTTOURNAMENTWIDGET_H
#define RECENTTOURNAMENTWIDGET_H

#include <QWidget>

namespace Ui
{
class RecentTournamentWidget;
}

class RecentTournamentWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RecentTournamentWidget(QWidget *parent = nullptr);
    ~RecentTournamentWidget();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::RecentTournamentWidget *ui;
};

#endif // RECENTTOURNAMENTWIDGET_H
