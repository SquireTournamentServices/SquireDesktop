#pragma once
#include <QWidget>
#include "../../../config.h"

namespace Ui
{
class SettingTab;
}

class SettingTab : public QWidget
{
    Q_OBJECT

public:
    explicit SettingTab(config_t *c, QWidget *parent = nullptr);
    ~SettingTab();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::SettingTab *ui;
    config_t *c;
};
