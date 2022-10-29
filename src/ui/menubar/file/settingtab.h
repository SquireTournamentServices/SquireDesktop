#pragma once
#include <QWidget>
#include <QAbstractButton>
#include "../../../config.h"
#include "../../abstracttabwidget.h"

namespace Ui
{
class SettingTab;
}

class SettingTab : public AbstractTabWidget
{
    Q_OBJECT

public:
    explicit SettingTab(config_t *c, QWidget *parent = nullptr);
    ~SettingTab();
protected:
    void changeEvent(QEvent *e);
public slots:
    bool canExit() override;
private:
    Ui::SettingTab *ui;
    bool changed;
    config_t *c;
    void uiSetSettings();
private slots:
    void onSave();
    void onReset();
    void onButtonClick(QAbstractButton* button); // Triggers the above
    void selectTournFolder();
    void onError(QString err);
    void onChange();
};
