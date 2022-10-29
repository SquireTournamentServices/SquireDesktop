#include "./tournamentunsavederrordialogue.h"
#include "./ui_tournamentunsavederrordialogue.h"
#include <QMessageBox>

TournamentUnsavedErrorDialogue::TournamentUnsavedErrorDialogue(Tournament *tourn, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TournamentUnsavedErrorDialogue)
{
    ui->setupUi(this);

    this->tourn = tourn;
    this->setWindowTitle(tr("Cannot save tournament ") + QString::fromStdString(tourn->name()));

    connect(ui->buttonBox, &QDialogButtonBox::clicked, this, &TournamentUnsavedErrorDialogue::clicked);
}

TournamentUnsavedErrorDialogue::~TournamentUnsavedErrorDialogue()
{
    delete ui;
}

void TournamentUnsavedErrorDialogue::clicked(QAbstractButton *button)
{
    switch (ui->buttonBox->buttonRole(button)) {
    case QDialogButtonBox::AcceptRole:
        this->onSave();
        break;
    case QDialogButtonBox::RejectRole:
        this->onCancel();
        break;
    case QDialogButtonBox::DestructiveRole:
        this->onDiscard();
        break;
    }
}

void TournamentUnsavedErrorDialogue::changeEvent(QEvent *e)
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

void TournamentUnsavedErrorDialogue::onSave()
{
    if (this->tourn->save()) {
        this->done(1);
    } else {
        QMessageBox msg = QMessageBox();
        msg.setWindowTitle(tr("Cannot save tournament ") + QString::fromStdString(this->tourn->name()) + ".");
        msg.setText(tr("Cannot save tournament."));
        msg.exec();
    }
}

void TournamentUnsavedErrorDialogue::onCancel()
{
    this->done(0);
}

void TournamentUnsavedErrorDialogue::onDiscard()
{
    this->done(1);
}

