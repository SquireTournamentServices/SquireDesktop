#include "addplayerdialogue.h"
#include "ui_addplayerdialogue.h"
#include <QPushButton>

AddPlayerDialogue::AddPlayerDialogue(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddPlayerDialogue)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("Add New Player To Tournament"));

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &AddPlayerDialogue::onOkay);
    connect(ui->nameEdit, &QLineEdit::textChanged, this, &AddPlayerDialogue::nameChanged);
    this->nameChanged(ui->nameEdit->text());
}

AddPlayerDialogue::~AddPlayerDialogue()
{
    delete ui;
}

void AddPlayerDialogue::changeEvent(QEvent *e)
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

void AddPlayerDialogue::nameChanged(QString text)
{
    QPushButton *button = ui->buttonBox->button(QDialogButtonBox::Ok);
    if (text == "") {
        button->setEnabled(false);
    } else {
        button->setEnabled(true);
    }
}

void AddPlayerDialogue::onOkay()
{
    std::string name = ui->nameEdit->text().toStdString();
    emit this->onAddPlayer(name);
    emit this->accept();
}

