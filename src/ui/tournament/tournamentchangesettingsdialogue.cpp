#include "tournamentchangesettingsdialogue.h"
#include "ui_tournamentchangesettingsdialogue.h"
#include "../../../testing_h/logger.h"
#include <QMessageBox>
#include <squire_ffi/squire_ffi.h>

TournamentChangeSettingsDialogue::TournamentChangeSettingsDialogue(Tournament *tourn, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TournamentChangeSettingsDialogue)
{
    ui->setupUi(this);
    this->tourn = tourn;
    this->setWindowTitle(tr("Changing Settings For ") + QString::fromStdString(this->tourn->name()));
    this->possibleStatusChanges = this->tourn->availableStatusChanges();
    for (squire_ffi::sc_TournamentStatus s : this->possibleStatusChanges) {
        QString str = this->tourn->statusToActionName(s);
        ui->statusEdit->addItem(str);
    }

    // Set the initial data
    ui->formatEdit->setText(QString::fromStdString(this->tourn->format()));
    ui->startingTableNumberEdit->setValue(this->tourn->starting_table_number());
    ui->useTableNumbersEdit->setChecked(this->tourn->use_table_number());
    ui->matchSizeEdit->setValue(this->tourn->game_size());
    ui->minDeckCountEdit->setValue(this->tourn->min_deck_count());
    ui->maxDeckCountEdit->setValue(this->tourn->max_deck_count());
    ui->allowRegistrationEdit->setChecked(this->tourn->reg_open());
    ui->requireCheckinsEdit->setChecked(this->tourn->require_check_in());
    ui->matchLengthEdit->setValue(this->tourn->round_length() / 60);

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
    int matchLength = ui->matchLengthEdit->value() * 60;
    bool regOpen = ui->allowRegistrationEdit->isChecked();
    bool requireCheckIn = ui->requireCheckinsEdit->isChecked();
    bool requireDeckReg = minDeckCount > 0;

    squire_ffi::sc_TournamentStatus newStatus = this->possibleStatusChanges[ui->statusEdit->currentIndex()];

    bool res = this->tourn->updateSettings(format,
                                           startingTableNumber,
                                           useTableNumber,
                                           gameSize,
                                           minDeckCount,
                                           maxDeckCount,
                                           matchLength,
                                           regOpen,
                                           requireCheckIn,
                                           requireDeckReg);
    if (!res) {
        lprintf(LOG_ERROR, "Cannot change tournament settings\n");
    }

    if (res && newStatus != this->tourn->status()) {
        switch (newStatus) {
        case squire_ffi::sc_TournamentStatus::Started:
            res |= this->tourn->start();
            break;
        case squire_ffi::sc_TournamentStatus::Ended:
            res |= this->tourn->end();
            break;
        case squire_ffi::sc_TournamentStatus::Cancelled:
            res |= this->tourn->cancel();
            break;
        case squire_ffi::sc_TournamentStatus::Frozen:
            res |= this->tourn->freeze();
        }

        if (!res) {
            lprintf(LOG_ERROR, "Cannot change tournament status\n");
        }
    }

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

