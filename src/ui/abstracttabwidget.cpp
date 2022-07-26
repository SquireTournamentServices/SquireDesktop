#include "./abstracttabwidget.h"
#include "../../testing_h/logger.h"

AbstractTabWidget::AbstractTabWidget(QWidget *parent)
    : QWidget(parent)
{

}

AbstractTabWidget::~AbstractTabWidget()
{

}

bool AbstractTabWidget::canExit()
{
    lprintf(LOG_WARNING, "Impropper tab detected!\n");
    return true;
}

