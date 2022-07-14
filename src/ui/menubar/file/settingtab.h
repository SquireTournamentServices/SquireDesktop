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
public slots:
    void onExit();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::SettingTab *ui;
    config_t *c;
private slots:
    void onSave();
    void onReset();
    void onRestore();
};
