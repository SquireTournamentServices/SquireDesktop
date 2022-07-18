#include "./labelimage.h"

void LabelImage::resizeEvent(QResizeEvent *pQEvent)
{
    QLabel::resizeEvent(pQEvent);
    setPixmap(_qPixmap, pQEvent->size());
}

void LabelImage::setPixmap(const QPixmap &qPixmap, const QSize &size)
{
    _qPixmap = qPixmap;
    _qPixmapScaled = _qPixmap.scaled(size, Qt::KeepAspectRatio);
    QLabel::setPixmap(_qPixmapScaled);
}
