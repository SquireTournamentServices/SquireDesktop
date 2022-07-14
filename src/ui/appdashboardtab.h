#pragma once
#include <QWidget>
#include <QTimer>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include "../config.h"

namespace Ui
{
class AppDashboardTab;
}

class AppDashboardTab : public QWidget
{
    Q_OBJECT

public:
    explicit AppDashboardTab(config_t t, QWidget *parent = nullptr);
    ~AppDashboardTab();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::AppDashboardTab *ui;
    QTimer resizeTimer;
    QGraphicsPixmapItem *img;
    QGraphicsScene* scene;
    QImage image;
private slots:
    void resizeImage();
};

