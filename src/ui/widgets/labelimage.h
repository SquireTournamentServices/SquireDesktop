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
    LabelImage(QWidget *parent = nullptr);
    ~LabelImage();
    void setPixmap(const QPixmap &qPixmap);
protected:
    void resizeEvent(QResizeEvent *pQEvent) override;
private:
    QPixmap pixmap;
    QPixmap pixmapScaled;
    void setPixmap(const QPixmap &qPixmap, const QSize &size);
};

