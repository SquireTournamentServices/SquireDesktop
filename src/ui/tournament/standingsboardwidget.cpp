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

    connect(this->table->selectionModel(), &QItemSelectionModel::selectionChanged, this, &StandingsBoardWidget::onPlayerSelected);
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

void StandingsBoardWidget::onPlayerSelected(const QItemSelection &selected, const QItemSelection deselected)
{
    QModelIndexList indexes = selected.indexes();
    if (indexes.size() > 0) {
        int index = indexes[0].row();
        Player plyr = this->table->getDataAt(index).player();
        emit this->playerSelected(plyr);
    }
}
