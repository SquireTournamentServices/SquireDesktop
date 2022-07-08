#include "settingtab.h"
#include "ui_settingtab.h"

SettingTab::SettingTab(config_t *c, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingTab)
{
    ui->setupUi(this);

    this->c = c;
}

SettingTab::~SettingTab()
{
    delete ui;
}

void SettingTab::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
