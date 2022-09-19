#include "roundresultwidget.h"
#include "ui_roundresultwidget.h"

RoundResultWidget::RoundResultWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RoundResultWidget)
{
    ui->setupUi(this);
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
