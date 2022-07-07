#pragma once
#include <QDialog>

namespace Ui {
  class CoinsFlipDialogue;
}

class CoinsFlipDialogue : public QDialog
{
  Q_OBJECT

public:
  explicit CoinsFlipDialogue(QWidget *parent = nullptr);
  ~CoinsFlipDialogue();

protected:
  void changeEvent(QEvent *e);

private:
  Ui::CoinsFlipDialogue *ui;
private slots:
  void onOkay();
};

