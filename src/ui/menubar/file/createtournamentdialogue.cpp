#include "createtournamentdialogue.h"
#include "ui_createtournamentdialogue.h"

CreateTournamentDialogue::CreateTournamentDialogue(QWidget *parent, config_t *config) :
  QDialog(parent),
  ui(new Ui::CreateTournamentDialogue)
{
  ui->setupUi(this);

  this->setWindowTitle(tr("New Tournament"));

  // Set defaults


  // Set name edit -> override save path

  // Set buttons
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
