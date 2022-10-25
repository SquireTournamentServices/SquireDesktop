#include "./standingsboardwidget.h"
#include "./ui_standingsboardwidget.h"

StandingsBoardWidget::StandingsBoardWidget(Tournament *tourn, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StandingsBoardWidget)
{
    ui->setupUi(this);

    this->tourn = tourn;
    this->setWindowTitle(QString::fromStdString(this->tourn->name()) + tr(" Standings"));

    // Init stuff
    this->tableLayout = new QVBoxLayout(ui->standingWidget);
    this->tableLayout->setAlignment(Qt::AlignTop);

    std::vector<PlayerScore> playerScores;
    this->table = new SearchSortTableWidget<PlayerScoreModel, PlayerScore>(playerScores);
    this->tableLayout->addWidget(this->table);

    this->redrawStandingsBoard();
    connect(this->tourn, &Tournament::onRoundsChanged, this, &StandingsBoardWidget::roundsChanged);
    connect(this->tourn, &Tournament::onPlayersChanged, this, &StandingsBoardWidget::playersChanged);
}

StandingsBoardWidget::~StandingsBoardWidget()
{
    delete ui;
}

void StandingsBoardWidget::changeEvent(QEvent *e)
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

void StandingsBoardWidget::redrawStandingsBoard()
{
    this->table->setData(this->tourn->standings());
}

void StandingsBoardWidget::roundsChanged(std::vector<Round>)
{
    this->redrawStandingsBoard();
}

void StandingsBoardWidget::playersChanged(std::vector<Player>)
{
    this->redrawStandingsBoard();
}
