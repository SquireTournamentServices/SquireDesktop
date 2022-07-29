#pragma once
#include <QString>
#include <QWidget>
#include <QtGlobal>
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
    void loadTournament(QString name);
protected:
    void changeEvent(QEvent *e);
#if QT_VERSION >= 0x060000
#include <QEnterEvent>
    void enterEvent(QEnterEvent *event) override;
#else
    void enterEvent(QEvent *event) override;
#endif
    void leaveEvent(QEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    Ui::RecentTournamentWidget *ui;
    recent_tournament_t t;
    LabelImage *img;
};

