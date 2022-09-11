#include "./labelimage.h"

LabelImage::LabelImage(QWidget *parent)
    : QLabel(parent)
{

}

LabelImage::~LabelImage()
{

}

void LabelImage::resizeEvent(QResizeEvent *pQEvent)
{
    QLabel::resizeEvent(pQEvent);
    this->setPixmap(this->pixmap, pQEvent->size());
}

void LabelImage::setPixmap(const QPixmap &qPixmap, const QSize &size)
{
    this->pixmap = qPixmap;
    this->pixmapScaled = this->pixmap.scaled(size, Qt::KeepAspectRatio);
    QLabel::setPixmap(this->pixmapScaled);
}

void LabelImage::setPixmap(const QPixmap &qPixmap)
{
    this->setPixmap(qPixmap, this->size());
}

