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
    void resizeEvent(QResizeEvent *pQEvent) override;

private:
    void setPixmap(const QPixmap &qPixmap, const QSize &size);
};

