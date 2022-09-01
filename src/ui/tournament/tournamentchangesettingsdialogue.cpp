#include "tournamentchangesettingsdialogue.h"
#include "ui_tournamentchangesettingsdialogue.h"
#include <QMessageBox>
#include <squire_core/squire_core.h>

TournamentChangeSettingsDialogue::TournamentChangeSettingsDialogue(Tournament *tourn, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TournamentChangeSettingsDialogue)
{
    ui->setupUi(this);
    this->tourn = tourn;

    this->setWindowTitle(tr("Changing Settings For ") + QString::fromStdString(this->tourn->name()));

    // Set the initial data
    ui->formatEdit->setText(QString::fromStdString(this->tourn->format()));
    ui->startingTableNumberEdit->setValue(this->tourn->starting_table_number());
    ui->useTableNumbersEdit->setChecked(this->tourn->use_table_number());
    ui->matchSizeEdit->setValue(this->tourn->game_size());
    ui->minDeckCountEdit->setValue(this->tourn->min_deck_count());
    ui->maxDeckCountEdit->setValue(this->tourn->max_deck_count());
    ui->allowRegistrationEdit->setChecked(this->tourn->reg_open());
    ui->requireCheckinsEdit->setChecked(this->tourn->require_check_in());

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &TournamentChangeSettingsDialogue::onApply);
}

TournamentChangeSettingsDialogue::~TournamentChangeSettingsDialogue()
{
    delete ui;
}

void TournamentChangeSettingsDialogue::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void TournamentChangeSettingsDialogue::onApply()
{
    std::string format = ui->formatEdit->text().toStdString();
    int startingTableNumber = ui->startingTableNumberEdit->value();
    bool useTableNumber = ui->useTableNumbersEdit->isChecked();
    int gameSize = ui->matchSizeEdit->value();
    int minDeckCount = ui->minDeckCountEdit->value();
    int maxDeckCount = ui->maxDeckCountEdit->value();
    bool regOpen = ui->allowRegistrationEdit->isChecked();
    bool requireCheckIn = ui->requireCheckinsEdit->isChecked();
    bool requireDeckReg = minDeckCount > 0;

    bool res = this->tourn->updateSettings(format,
                                           startingTableNumber,
                                           useTableNumber,
                                           gameSize,
                                           minDeckCount,
                                           maxDeckCount,
                                           regOpen,
                                           requireCheckIn,
                                           requireDeckReg);
    if (res) {
        this->accept();
    } else {
        QMessageBox dlg = QMessageBox();
        dlg.setText(tr("Cannot update settings."));
        dlg.setWindowTitle(tr("Cannot update settings for ")
                           + QString::fromStdString(tourn->name())
                           + ".");
        dlg.exec();
    }
}

