#include "settingtab.h"
#include "ui_settingtab.h"
#include <string>
#include <string.h>

SettingTab::SettingTab(config_t *c, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingTab)
{
    ui->setupUi(this);

    // Init state
    this->c = c;
    this->uiSetSettings();
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

void SettingTab::uiSetSettings()
{
    ui->tournSaveLocation->setText(QString::fromStdString(std::string(c->tourn_save_path)));

    ui->deckCountSpinBox->setValue(c->default_settings.deck_count);
    ui->matchSizeSpinBox->setValue(c->default_settings.match_size);

    ui->drawPointsSpinBox->setValue(c->default_settings.points_draw);
    ui->byePointsSpinBox->setValue(c->default_settings.points_bye);
    ui->winPointsSpinBox->setValue(c->default_settings.points_win);
    ui->lossPointsSpinBox->setValue(c->default_settings.points_loss);

    switch (c->default_settings.type) {
    case SWISS_TOURN:
        ui->swissRadio->setChecked(true);
        break;
    case FLUID_TOURN:
        ui->fluidRadio->setChecked(true);
        break;
    }
}

void SettingTab::onExit()
{

}

void SettingTab::onSave()
{
    // Load settings in the tab to a temp settings struct
    config_t tmp_config;

    // Free old config then copy new config in
    free_config(this->c);
    memcpy(this->c, &tmp_config, sizeof(tmp_config));

}

void SettingTab::onReset()
{

}

void SettingTab::onRestore()
{

}

