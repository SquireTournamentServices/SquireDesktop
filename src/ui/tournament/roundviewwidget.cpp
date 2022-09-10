#include "./roundviewwidget.h"
#include "./ui_roundviewwidget.h"

RoundViewWidget::RoundViewWidget(Tournament *tourn, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RoundViewWidget)
{
    ui->setupUi(this);
    this->tourn = tourn;
    this->roundSelected = false;

    // Init the tables
    this->playerTableLayout = new QVBoxLayout(ui->playerInRoundTable);
    this->playerTableLayout->setAlignment(Qt::AlignTop);

    std::vector<Player> players;
    this->playerTable = new SearchSortTableWidget<PlayerModel, Player>(players);
    this->playerTableLayout->addWidget(playerTable);

    connect(this->tourn, &Tournament::onPlayersChanged, this, &RoundViewWidget::onPlayersChanged);
    connect(&this->timeLeftUpdater, &QTimer::timeout, this, &RoundViewWidget::displayRound);
    this->displayRound(); // calls displayRound
}

RoundViewWidget::~RoundViewWidget()
{
    delete playerTable;
    delete playerTableLayout;
    delete ui;
}

void RoundViewWidget::changeEvent(QEvent *e)
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

QString RoundViewWidget::matchNumberToStr(int number)
{
    QString base = tr("Match #");
    if (number < 10) {
        base += tr("0");
    }

    base += QString::number(number, 10);

    return base;
}

void RoundViewWidget::displayRound()
{
    // Genereate state strings
    QString statusStr = tr("No Round Selected");
    QString numberStr = tr("Match #--");
    int timeLeft = 0;
    int duration = 0;

    if (this->roundSelected) {
        this->playerTable->setData(this->round.players());
        numberStr = matchNumberToStr(this->round.match_number());
        timeLeft = this->round.time_left();
        duration = this->round.duration();

        // Status
        switch(this->round.status()) {
        case squire_core::sc_RoundStatus::Open:
            statusStr = tr("Round is in progress");
            break;
        case squire_core::sc_RoundStatus::Uncertified:
            statusStr = tr("Round is waiting results certification");
            break;
        case squire_core::sc_RoundStatus::Certified:
            statusStr = tr("Round has been finished and, results are confirmed");
            break;
        case squire_core::sc_RoundStatus::Dead:
            statusStr = tr("Round has been deleted");
            break;
        }
    } else {
        this->playerTable->setData(std::vector<Player>());
    }

    // Timer
    QString timeLeftStr = "";
    if (timeLeft == 0) {
        timeLeftStr = tr("Round has ended");
    } else {
        int seconds = timeLeft % 60;
        int minutes = ((timeLeft / 60) % 60);
        int hours = timeLeft / (60 * 60);

        if (hours < 10) {
            timeLeftStr += "0";
        }
        timeLeftStr += QString::number(hours);
        timeLeftStr += ":";

        if (minutes < 10) {
            timeLeftStr += "0";
        }
        timeLeftStr += QString::number(minutes);
        timeLeftStr += ":";

        if (seconds < 10) {
            timeLeftStr += "0";
        }
        timeLeftStr += QString::number(seconds);
        timeLeftStr += " " + tr("Left in Round");
    }

    // Render values in GUI
    ui->matchNumber->setText(numberStr);
    ui->roundStatus->setText(statusStr);
    ui->timeLeftLabel->setText(timeLeftStr);
    ui->timeExtensionEdit->setValue(0);

    if (duration == 0) {
        ui->timeLeftProgressBar->setValue(0);
    } else {
        ui->timeLeftProgressBar->setValue((100 * timeLeft) / duration);
    }

    this->timeLeftUpdater.start(100);
}

void RoundViewWidget::onPlayersChanged(std::vector<Player>)
{
    this->displayRound();
}

void RoundViewWidget::setRound(Round round)
{
    this->round = round;
    this->roundSelected = true;
    this->displayRound();
}

