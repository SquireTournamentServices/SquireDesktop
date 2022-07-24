#pragma once
#include <QWidget>
#include <QVBoxLayout>
#include "../config.h"
#include "./widgets/labelimage.h"

namespace Ui
{
class AppDashboardTab;
}

class AppDashboardTab : public QWidget
{
    Q_OBJECT

public:
    explicit AppDashboardTab(config_t t, QWidget *parent = nullptr);
    ~AppDashboardTab();
    Ui::AppDashboardTab *ui; // Sorry OOP fans

    void onTournamentAdded(recent_tournament_t t);
protected:
    void changeEvent(QEvent *e);

private:
    LabelImage *banner;
    QVBoxLayout *layout;
};

