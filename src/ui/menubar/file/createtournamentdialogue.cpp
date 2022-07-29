#include "./createtournamentdialogue.h"
#include "./ui_createtournamentdialogue.h"
#include "./random_names.h"
#include "../../../ffi_utils.h"
#include "../../../io_utils.h"
#include <QFileDialog>
#include <QMessageBox>

CreateTournamentDialogue::CreateTournamentDialogue(config_t *config, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateTournamentDialogue)
{
    ui->setupUi(this);

    this->config = config;
    this->setWindowTitle(tr("New Tournament"));

    // Set name edit -> override save path
    connect(ui->nameEdit, &QLineEdit::textChanged, this, &CreateTournamentDialogue::nameEditChanged);

    // Set defaults
    ui->nameEdit->setText(QString::fromStdString(get_random_name()));
    ui->formatEdit->setText("cEDH");

    switch (this->config->default_settings.type) {
    case SWISS_TOURN:
        ui->swissRadio->setChecked(true);
        break;
    case FLUID_TOURN:
        ui->fluidRoundRadio->setChecked(true);
        break;
    };

    ui->minDeckCount->setValue(this->config->default_settings.min_deck_count);
    ui->maxDeckCount->setValue(this->config->default_settings.max_deck_count);
    ui->matchSize->setValue(this->config->default_settings.match_size);

    ui->pointsWin->setValue(this->config->default_settings.points_win);
    ui->pointsLoss->setValue(this->config->default_settings.points_loss);
    ui->pointsBye->setValue(this->config->default_settings.points_bye);
    ui->pointsDraw->setValue(this->config->default_settings.points_draw);

    // Set buttons
    connect(ui->saveLocationButton, &QPushButton::clicked, this, &CreateTournamentDialogue::changeSaveLocation);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &CreateTournamentDialogue::onOkay);
}

CreateTournamentDialogue::~CreateTournamentDialogue()
{
    delete ui;
}

void CreateTournamentDialogue::changeEvent(QEvent *e)
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

std::string CreateTournamentDialogue::getName()
{
    return std::string(this->config->tourn_save_path) +
           "/" +
           ui->nameEdit->text().toStdString() +
           TOURNAMENT_EXTENTION;
}

void CreateTournamentDialogue::nameEditChanged(QString name)
{
    ui->saveLocation->setText(QString::fromStdString(this->getName()));
}

void CreateTournamentDialogue::changeSaveLocation()
{
    std::string tmp = ui->saveLocation->text().toStdString();
    std::string dir = tmp;
    std::string file = tmp;
    size_t index = tmp.find_last_of("/");
    if (index != std::string::npos) {
        dir = tmp.substr(0, index);
        file = tmp.substr(index + 1, tmp.size());
    }

    QFileDialog dlg;
    dlg.setWindowTitle(tr("Save Tournament As"));
    dlg.setFileMode(QFileDialog::AnyFile);
    dlg.setDirectory(QString::fromStdString(dir));
    dlg.selectFile(QString::fromStdString(file));
    dlg.setViewMode(QFileDialog::Detail);
    dlg.setNameFilter(tr("All Squire Tournament files") + QString(" (*.json *" TOURNAMENT_EXTENTION ")"));

    if (dlg.exec()) {
        ui->saveLocation->setText(dlg.directory().absolutePath() + dlg.selectedFiles().at(0));
    } else {
        QApplication::beep();
    }
}

void CreateTournamentDialogue::onOkay()
{
    if (ui->minDeckCount->value() > ui->maxDeckCount->value()) {
        ui->maxDeckCount->setValue(ui->minDeckCount->value());
        return;
    }

    squire_core::sc_TournamentPreset preset = squire_core::sc_TournamentPreset::Swiss;
    if (ui->fluidRoundRadio->isChecked()) {
        preset = squire_core::sc_TournamentPreset::Fluid;
    }

    init_tourn_folder(this->config);

    Tournament tourn;
    bool valid = new_tournament(ui->saveLocation->text().toStdString(),
                                ui->nameEdit->text().toStdString(),
                                ui->formatEdit->text().toStdString(),
                                preset,
                                ui->useTableNumbers->isChecked(),
                                ui->matchSize->value(),
                                ui->minDeckCount->value(),
                                ui->maxDeckCount->value(),
                                ui->allowPlayerRegistration->isChecked(),
                                ui->requirePlayerCheckins->isChecked(),
                                ui->minDeckCount->value() == 0,
                                &tourn);
    if (valid) {
        lprintf(LOG_INFO, "Created tournament %s\n", ui->saveLocation->text().toStdString().c_str());

        this->onTournamentAdded(tourn); // This frees the malloc above.
        emit this->close();
    } else {
        QMessageBox dlg(this);
        dlg.setText(tr("Error making new tournament"));
        dlg.setText(tr("Cannot save new tournament"));
        dlg.exec();
    }
}

