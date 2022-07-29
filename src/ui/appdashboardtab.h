#pragma once
#include <QWidget>
#include <QVBoxLayout>
#include "../config.h"
#include "./widgets/labelimage.h"
#include "./abstracttabwidget.h"

namespace Ui
{
class AppDashboardTab;
}

class AppDashboardTab : public AbstractTabWidget
{
    Q_OBJECT

public:
    explicit AppDashboardTab(config_t t, QWidget *parent = nullptr);
    ~AppDashboardTab();
    Ui::AppDashboardTab *ui; // Sorry OOP fans

    void onTournamentAdded(recent_tournament_t t);
signals:
    void loadTournament(QString name);
public slots:
    bool canExit() override;
protected:
    void changeEvent(QEvent *e);
private:
    LabelImage *banner;
    QVBoxLayout *layout;
private slots:
    void openTournament(QString name);
};

