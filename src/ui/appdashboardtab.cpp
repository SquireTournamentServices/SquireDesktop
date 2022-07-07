#include "appdashboardtab.h"
#include "ui_appdashboardtab.h"

AppDashboardTab::AppDashboardTab(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::AppDashboardTab)
{
  ui->setupUi(this);
}

AppDashboardTab::~AppDashboardTab()
{
  delete ui;
}

void AppDashboardTab::changeEvent(QEvent *e)
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
