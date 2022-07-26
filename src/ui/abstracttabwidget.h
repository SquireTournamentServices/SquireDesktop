#pragma once
#include <QWidget>

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
};

