#pragma once
#include <vector>
#include <QWidget>
#include <QMenu>

class AbstractTabWidget : public QWidget
{
    Q_OBJECT

signals:
    /*
     * This is emitted when the tab wants to close itself.
     * */
    void close();
public:
    AbstractTabWidget(QWidget *parent=nullptr);
    ~AbstractTabWidget();
public slots:
    /*
     * Returns true if the tab can be killed, false if it cannot.
     * */
    virtual bool canExit();
    /*
     * Returns the menu that the tab uses.
     */
    std::vector<QMenu *> getMenus();
protected:
    std::vector<QMenu *> menus;
    QMenu *addMenu(QString name);
};

