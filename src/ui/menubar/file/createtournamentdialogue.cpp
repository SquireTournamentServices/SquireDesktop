#include "createtournamentdialogue.h"
#include "ui_createtournamentdialogue.h"

createtournamentdialogue::createtournamentdialogue(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::createtournamentdialogue)
{
  ui->setupUi(this);
}

createtournamentdialogue::~createtournamentdialogue()
{
  delete ui;
}

void createtournamentdialogue::changeEvent(QEvent *e)
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
