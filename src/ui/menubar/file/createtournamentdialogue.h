#pragma once
#include "../../../config.h"
#include <QDialog>

namespace Ui {
  class CreateTournamentDialogue;
}

class CreateTournamentDialogue : public QDialog
{
  Q_OBJECT

public:
  explicit CreateTournamentDialogue(QWidget *parent = nullptr, config_t *config);
  ~CreateTournamentDialogue();

protected:
  void changeEvent(QEvent *e);

private:
  Ui::CreateTournamentDialogue *ui;
};

