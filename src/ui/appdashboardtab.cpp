#include "./appdashboardtab.h"
#include "./ui_appdashboardtab.h"
#include "./recenttournamentwidget.h"
#include "assets.h"
#include <QApplication>

AppDashboardTab::AppDashboardTab(config_t t, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AppDashboardTab)
{
    ui->setupUi(this);

    // Set recent tournaments
    QVBoxLayout *layout = new QVBoxLayout(ui->recentTournaments);
    layout->setAlignment(Qt::AlignTop);
    for (int i = 0; i < t.recent_tournament_count; i++) {
        RecentTournamentWidget *w = new RecentTournamentWidget(t.recent_tournaments[i], this);
        layout->addWidget(w);
    }

    // Banner stuff
    this->image.loadFromData(BANNER_PNG, sizeof(BANNER_PNG));
    this->img = new QGraphicsPixmapItem(QPixmap::fromImage(this->image));
    this->scene = new QGraphicsScene;

    this->scene->addItem(this->img);
    this->img->setPos(0, 0);
    ui->bannerView->setScene(scene);

    this->resizeImage();
    connect(&this->resizeTimer, &QTimer::timeout, this, &AppDashboardTab::resizeImage);
}

AppDashboardTab::~AppDashboardTab()
{
    delete ui;
    delete this->img;
    delete this->scene;
}

void AppDashboardTab::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void AppDashboardTab::resizeImage()
{
    double w = this->image.width();
    double h = this->image.height();

    if (w == 0 || h ==0) {
        return;
    }

    double scale1 = ui->bannerView->width() / w;
    double scale2 = ui->bannerView->height() / h;

    // Min
    double scale = scale1;
    if (scale2 < scale1) {
        scale = scale2;
    }

    // Set scale
    this->img->setScale(scale);
    this->img->setPos(0, 0);

    this->resizeTimer.start(100);
}

