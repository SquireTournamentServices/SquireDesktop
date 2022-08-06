#pragma once
#include <QDialog>
#include <string>
#include "../../model/abstract_tournament.h"

namespace Ui
{
class AddPlayerDialogue;
}

class AddPlayerDialogue : public QDialog
{
    Q_OBJECT

signals:
    void onAddPlayer(std::string name);
public:
    explicit AddPlayerDialogue(QWidget *parent = nullptr);
    ~AddPlayerDialogue();
private slots:
    void onOkay();
    void nameChanged(QString text);
protected:
    void changeEvent(QEvent *e);
private:
    Ui::AddPlayerDialogue *ui;
};

