#include "./settingtab.h"
#include "./ui_settingtab.h"
#include "../../../io_utils.h"
#include <QMessageBox>
#include <QApplication>
#include <QFileDialog>
#include <string>
#include <string.h>

#define QSPINBOX_INT QOverload<int>::of(&QSpinBox::valueChanged)

SettingTab::SettingTab(config_t *c, QWidget *parent) :
    AbstractTabWidget(parent),
    ui(new Ui::SettingTab)
{
    ui->setupUi(this);

    // Init state
    this->c = c;
    this->uiSetSettings();
    this->changed = false;

    // Connect change events
    connect(ui->tournSaveLocation, &QLineEdit::textChanged, this, [this](QString text) {
        this->onChange();
    });

    connect(ui->minDeckCountSpinBox, QSPINBOX_INT, this, [this](int v) {
        this->onChange();
    });
    connect(ui->maxDeckCountSpinBox, QSPINBOX_INT, this, [this](int v) {
        this->onChange();
    });
    connect(ui->matchSizeSpinBox, QSPINBOX_INT, this, [this](int v) {
        this->onChange();
    });

    connect(ui->drawPointsSpinBox, QSPINBOX_INT, this, [this](int v) {
        this->onChange();
    });
    connect(ui->byePointsSpinBox, QSPINBOX_INT, this, [this](int v) {
        this->onChange();
    });
    connect(ui->winPointsSpinBox, QSPINBOX_INT, this, [this](int v) {
        this->onChange();
    });
    connect(ui->lossPointsSpinBox, QSPINBOX_INT, this, [this](int v) {
        this->onChange();
    });

    connect(ui->swissRadio, &QAbstractButton::clicked, this, &SettingTab::onChange);
    connect(ui->fluidRadio, &QAbstractButton::clicked, this, &SettingTab::onChange);

    // Connect the buttons
    connect(ui->buttonBox, &QDialogButtonBox::clicked, this, &SettingTab::onButtonClick);
    connect(ui->tournSaveLocationButton, &QPushButton::clicked, this, &SettingTab::selectTournFolder);
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

void SettingTab::onButtonClick(QAbstractButton* button)
{
    if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ResetRole) {
        this->onReset();
    }
    if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::AcceptRole) {
        this->onSave();
    }
}

void SettingTab::selectTournFolder()
{
    QFileDialog dialog(this, tr("Select the folder to save tournaments to."));
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setViewMode(QFileDialog::Detail);
    if (dialog.exec()) {
        ui->tournSaveLocation->setText(dialog.directory().absolutePath());
        this->onChange();
    } else {
        QApplication::beep();
    }
}

void SettingTab::uiSetSettings()
{
    ui->tournSaveLocation->setText(QString::fromStdString(std::string(c->tourn_save_path)));

    ui->minDeckCountSpinBox->setValue(c->default_settings.min_deck_count);
    ui->maxDeckCountSpinBox->setValue(c->default_settings.max_deck_count);
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

void SettingTab::onError(QString err)
{
    QMessageBox *dlg = new QMessageBox(this);
    dlg->setText(err);
    dlg->setWindowTitle(tr("An Error Occurred Saving Settings"));
    dlg->exec();
}

void SettingTab::onSave()
{
    // Load settings in the tab to a temp settings struct
    config_t tmp_config;
    memset(&tmp_config, 0, sizeof(tmp_config));

    // Read current settings into tmp_config
    tmp_config.tourn_save_path = clone_std_string(ui->tournSaveLocation->text().toStdString());

    tmp_config.default_settings.min_deck_count = ui->minDeckCountSpinBox->value();
    tmp_config.default_settings.max_deck_count = ui->maxDeckCountSpinBox->value();
    tmp_config.default_settings.match_size = ui->matchSizeSpinBox->value();

    tmp_config.default_settings.points_draw = ui->drawPointsSpinBox->value();
    tmp_config.default_settings.points_bye = ui->byePointsSpinBox->value();
    tmp_config.default_settings.points_loss = ui->lossPointsSpinBox->value();
    tmp_config.default_settings.points_win = ui->winPointsSpinBox->value();

    if (ui->swissRadio->isChecked()) {
        tmp_config.default_settings.type = SWISS_TOURN;
    } else if (ui->fluidRadio->isChecked()) {
        tmp_config.default_settings.type = FLUID_TOURN;
    }

    // Free old config then copy new config in
    free_config(this->c);
    memcpy(this->c, &tmp_config, sizeof(tmp_config));

    FILE *f = fopen(CONFIG_FILE, "w");
    if (f == NULL) {
        this->onError(tr("Cannot open file"));
        return;
    }

    bool v = write_config(this->c, f);
    if (!v) {
        this->onError(tr("Cannot encode settings"));
    } else {
        this->changed = false;
        lprintf(LOG_INFO, "Saved settings\n");
    }
    fclose(f);
}

void SettingTab::onReset()
{
    config_t tmp_config = DEFAULT_CONFIG;
    free_config(this->c);
    memcpy(this->c, &tmp_config, sizeof(tmp_config));

    this->uiSetSettings();
    this->onChange();
}

void SettingTab::onChange()
{
    this->changed = true;
}

bool SettingTab::canExit()
{
    if (this->changed) {
        // Ask the user if they want to save their settings
        QMessageBox msgBox;
        msgBox.setWindowTitle(tr("Unsaved Settings Changes"));
        msgBox.setText("Do you want to save your settings?");
        msgBox.setStandardButtons(QMessageBox::Save);
        msgBox.addButton(QMessageBox::No);
        msgBox.addButton(QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);

        int ret = msgBox.exec();
        if (ret == QMessageBox::Save) {
            this->onSave(); // No error handling.
            return true;
        } else if (ret == QMessageBox::Cancel) {
            return true;
        } else {
            return false;
        }
    } else {
        return true;
    }
}

