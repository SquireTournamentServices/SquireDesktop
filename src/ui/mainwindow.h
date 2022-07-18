#pragma once
#include <QMainWindow>
#include <QString>
#include "../config.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(config_t *t, QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    config_t *config;
    void addTab(QWidget *w, QString name);
private slots:
    void coinFlipUtility();
    void diceRollUtility();
    void settings();
    void closeTab(int index);
};
