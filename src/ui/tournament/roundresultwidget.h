#ifndef ROUNDRESULTWIDGET_H
#define ROUNDRESULTWIDGET_H

#include <QWidget>

namespace Ui
{
class RoundResultWidget;
}

class RoundResultWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RoundResultWidget(QWidget *parent = nullptr);
    ~RoundResultWidget();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::RoundResultWidget *ui;
};

#endif // ROUNDRESULTWIDGET_H
