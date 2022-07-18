#pragma once
#include <QApplication>
#include <QResizeEvent>
#include <QLabel>
#include <QMainWindow>
#include <QPixmap>
#include <QTimer>

class LabelImage: public QLabel
{

private:
    QPixmap _qPixmap, _qPixmapScaled;

public:
    void setPixmap(const QPixmap &qPixmap)
    {
        setPixmap(qPixmap, size());
    }

protected:
    virtual void resizeEvent(QResizeEvent *pQEvent);

private:
    void setPixmap(const QPixmap &qPixmap, const QSize &size);
};

