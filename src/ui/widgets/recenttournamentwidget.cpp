#include <time.h>
#include <string>
#include <assets.h>
#include <QStyle>
#include "./recenttournamentwidget.h"
#include "./ui_recenttournamentwidget.h"

RecentTournamentWidget::RecentTournamentWidget(recent_tournament_t t, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RecentTournamentWidget)
{
    ui->setupUi(this);

    this->t = t;
    ui->name->setText(QString(t.name == NULL ? "Error Loading": t.name));

    if (t.pairing_sys == NULL) {
        ui->type->setText(tr("N/a"));
    } else {
        ui->type->setText(QString::fromStdString(std::string(t.pairing_sys)));
    }

    if (t.name == NULL || t.pairing_sys == NULL) {
        ui->editTime->setText(tr("Error with: ") + QString(t.file_path));
        pixmap.loadFromData(WARNING_PNG, sizeof(WARNING_PNG));
    } else {
        char timeString[50];
        strftime(timeString, sizeof(timeString), "%x - %H:%M:%S %Z", &t.last_opened);
        ui->editTime->setText(QString(timeString));

        if (strcmp(t.pairing_sys, PAIRING_SWISS) == 0) {
            pixmap.loadFromData(SWISS_PNG, sizeof(SWISS_PNG));
        } else if (strcmp(t.pairing_sys, PAIRING_FLUID) == 0) {
            pixmap.loadFromData(FLUID_PNG, sizeof(FLUID_PNG));
        }
    }

    this->img = new LabelImage();
    this->img->setPixmap(pixmap);

    this->layout = new QVBoxLayout(ui->frame);
    this->layout->setAlignment(Qt::AlignTop);
    this->layout->addWidget(this->img);
}

RecentTournamentWidget::~RecentTournamentWidget()
{
    delete ui;
    delete layout;
    free(this->t.name);
    free(this->t.file_path);
    free(this->t.pairing_sys);
}

void RecentTournamentWidget::changeEvent(QEvent *e)
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

// Override QWidget
void RecentTournamentWidget::mousePressEvent(QMouseEvent *event)
{
    this->setBackgroundRole(QPalette::Link);
    emit this->loadTournament(QString(this->t.file_path));
}

#if QT_VERSION >= 0x060000
void RecentTournamentWidget::enterEvent(QEnterEvent *event)
#else
void RecentTournamentWidget::enterEvent(QEvent *event)
#endif
{
    this->setBackgroundRole(QPalette::Highlight);
}

void RecentTournamentWidget::leaveEvent(QEvent *event)
{
    this->setBackgroundRole(QPalette::Base);
}

void RecentTournamentWidget::mouseReleaseEvent(QMouseEvent *event)
{
    this->setBackgroundRole(QPalette::Base);
}

