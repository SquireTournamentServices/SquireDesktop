#include "roundviewwidget.h"
#include "ui_roundviewwidget.h"

RoundViewWidget::RoundViewWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RoundViewWidget)
{
    ui->setupUi(this);

    // Init the tables

    this->roundSelected = false;
    this->displayRound();
}

RoundViewWidget::~RoundViewWidget()
{
    delete ui;
}

void RoundViewWidget::changeEvent(QEvent *e)
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
    
QString RoundViewWidget::matchNumberToStr(int number)
{
    QString base = tr("Match #");
    if (number < 10) {
       base += tr("0");
    }

    base += QString.number(number, 10);

    return base;
}

void RoundViewWidget::displayRound()
{
    // Genereate state strings
    QString statusStr = tr("No Round Selected");
    QString numberStr = tr("Match #--");

    if (this->roundSelected) {

    }

    // Render values in GUI
}

void RoundViewWidget::setRound(Round round)
{
    this->round = round;
    this->roundSelected = true;
    this->displayRound();
}

