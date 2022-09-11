#include "playerviewwidget.h"
#include "ui_playerviewwidget.h"

PlayerViewWidget::PlayerViewWidget(Tournament *tourn, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlayerViewWidget)
{
    ui->setupUi(this);

    this->tourn = tourn;
    this->playerSelected = false;
    this->player = Player();

    // Init the tables
    this->roundTableLayout = new QVBoxLayout(ui->playersRounds);
    this->roundTableLayout->setAlignment(Qt::AlignTop);

    std::vector<Round> rounds;
    this->roundTable = new SearchSortTableWidget<RoundModel, Round>(rounds);
    this->roundTableLayout->addWidget(roundTable);

    connect(this->tourn, &Tournament::onPlayersChanged, this, &PlayerViewWidget::onPlayersChanged);
    connect(&this->timeLeftUpdater, &QTimer::timeout, this, &PlayerViewWidget::displayPlayer);
    connect(this->roundTable->selectionModel(), &QItemSelectionModel::selectionChanged, this, &PlayerViewWidget::onRoundSelected);

    this->displayPlayer();
}

PlayerViewWidget::~PlayerViewWidget()
{
    delete ui;
}

void PlayerViewWidget::changeEvent(QEvent *e)
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

void PlayerViewWidget::displayPlayer()
{
    if (this->playerSelected) {

    }
}

void PlayerViewWidget::setPlayer(Player player)
{
    this->player = player;
    this->playerSelected = true;
    this->displayPlayer();
}

void PlayerViewWidget::onRoundSelected(const QItemSelection &selected, const QItemSelection deselected)
{
    QModelIndexList indexes = selected.indexes();
    if (indexes.size() > 0) {
        int index = indexes[0].row();
        Round rnd = this->roundTable->getDataAt(index);
        emit this->roundSelected(rnd);
    }
}

void PlayerViewWidget::onPlayersChanged(std::vector<Player> players)
{
    this->displayPlayer();
}

