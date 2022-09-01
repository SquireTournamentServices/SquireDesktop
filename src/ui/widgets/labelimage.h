#pragma once
#include <QApplication>
#include <QResizeEvent>
#include <QLabel>
#include <QMainWindow>
#include <QPixmap>
#include <QTimer>

class LabelImage: public QLabel
{
public:
    void setPixmap(const QPixmap &qPixmap);
protected:
    void resizeEvent(QResizeEvent *pQEvent) override;
private:
    QPixmap _qPixmap, _qPixmapScaled;
    void setPixmap(const QPixmap &qPixmap, const QSize &size);
};

