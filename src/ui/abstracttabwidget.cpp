#include "./abstracttabwidget.h"
#include "../../testing_h/logger.h"

AbstractTabWidget::AbstractTabWidget(QWidget *parent)
    : QWidget(parent)
{
    this->menus = std::vector<QMenu *>();
}

AbstractTabWidget::~AbstractTabWidget()
{
    for (QMenu *menu : this->menus) {
        delete menu;
    }
}

bool AbstractTabWidget::canExit()
{
    return true;
}

std::vector<QMenu *> AbstractTabWidget::getMenus()
{
    return this->menus;
}

QMenu *AbstractTabWidget::addMenu(QString name)
{
    QMenu *ret = new QMenu(name);
    this->menus.push_back(ret);
    return ret;
}

