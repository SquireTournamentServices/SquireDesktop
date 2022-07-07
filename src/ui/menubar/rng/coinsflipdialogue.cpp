#include <QMessageBox>
#include "./coinsflipdialogue.h"
#include "./ui_coinsflipdialogue.h"
#include "../../../coins.h"

CoinsFlipDialogue::CoinsFlipDialogue(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CoinsFlipDialogue)
{
    ui->setupUi(this);
    this->setWindowTitle("Flip coins.");
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &CoinsFlipDialogue::onOkay);
}

CoinsFlipDialogue::~CoinsFlipDialogue()
{
    delete ui;
}

void CoinsFlipDialogue::changeEvent(QEvent *e)
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

void CoinsFlipDialogue::onOkay()
{
    // Flip dice
    int res;
    int coins = ui->spinBox->value();
    QString postfix;
    if (ui->krarkBox->checkState() == Qt::Checked) {
        res = flip_krark_coins(coins);
        postfix = tr(" Krark coins.");
    } else {
        res = flip_coins(coins);
        postfix = tr(" coins.");
    }

    QString str = tr("You flipped ") + QString::number(res, 10) + postfix;
    QMessageBox dlg(this);
    dlg.setText(str);
    dlg.setWindowTitle(tr("Coin Flip Results"));
    dlg.exec();
}
