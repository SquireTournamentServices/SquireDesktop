#include "roundresultwidget.h"
#include "ui_roundresultwidget.h"

RoundResultWidget::RoundResultWidget(RoundResults *results, Player player, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RoundResultWidget)
{
    ui->setupUi(this);

    this->player = player;
    this->results = results;

    ui->winSpinBox->setValue(results->resultFor(this->player));
    ui->playerName->setText(QString::fromStdString(this->player.all_names()));
    bool conf = results->isConfirmed(this->player);
    ui->confirmedIndicator->setDisabled(conf);
    ui->confirmedIndicator->setChecked(conf);
}

RoundResultWidget::~RoundResultWidget()
{
    delete ui;
}

void RoundResultWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

int RoundResultWidget::newWins()
{
    return ui->winSpinBox->value();
}

bool RoundResultWidget::confirmed()
{
    return ui->confirmedIndicator->isChecked();
}
