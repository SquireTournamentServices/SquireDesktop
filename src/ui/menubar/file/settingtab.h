#pragma once
#include <QWidget>
#include <QAbstractButton>
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
    bool canExit();

protected:
    void changeEvent(QEvent *e);

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
