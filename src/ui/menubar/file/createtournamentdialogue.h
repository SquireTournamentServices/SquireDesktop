#ifndef CREATETOURNAMENTDIALOGUE_H
#define CREATETOURNAMENTDIALOGUE_H

#include <QDialog>

namespace Ui {
  class createtournamentdialogue;
}

class createtournamentdialogue : public QDialog
{
  Q_OBJECT

public:
  explicit createtournamentdialogue(QWidget *parent = nullptr);
  ~createtournamentdialogue();

protected:
  void changeEvent(QEvent *e);

private:
  Ui::createtournamentdialogue *ui;
};

#endif // CREATETOURNAMENTDIALOGUE_H
