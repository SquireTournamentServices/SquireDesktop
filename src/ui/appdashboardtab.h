#pragma once
#include <QWidget>

namespace Ui {
  class AppDashboardTab;
}

class AppDashboardTab : public QWidget
{
  Q_OBJECT

public:
  explicit AppDashboardTab(QWidget *parent = nullptr);
  ~AppDashboardTab();

protected:
  void changeEvent(QEvent *e);

private:
  Ui::AppDashboardTab *ui;
};

