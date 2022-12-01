#include "./playerlistwidget.h"
#include "./ui_playerlistwidget.h"
#include "./addplayerdialogue.h"
#include <QMessageBox>

PlayerListWidget::PlayerListWidget(Tournament *tourn, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlayerListWidget)
{
    ui->setupUi(this);

    this->tourn = tourn;

    // Add player table
    this->playerTableLayout = new QVBoxLayout(ui->playerTable);
    this->playerTableLayout->setAlignment(Qt::AlignTop);

    std::vector<Player> players = this->tourn->players();
    this->playerTable = new SearchSortTableWidget<PlayerModel, Player>(players);

    QString showActivePlayers = tr("Only Show Active Players");
    this->playerTable->addAdditionalFilter(showActivePlayers.toStdString(), &playerIsActive);
    this->playerTableLayout->addWidget(playerTable);

    connect(this->tourn, &Tournament::onPlayerAdded, this, &PlayerListWidget::onPlayerAdded);
    connect(this->tourn, &Tournament::onPlayersChanged, this, &PlayerListWidget::onPlayersChanged);
    connect(this->tourn, &Tournament::onRegOpenChanged, this, &PlayerListWidget::onRegOpenChanged);
    connect(this->playerTable->selectionModel(), &QItemSelectionModel::selectionChanged, this, &PlayerListWidget::onPlayerSelected);

    connect(ui->addPlayer, &QPushButton::clicked, this, &PlayerListWidget::addPlayerClicked);
}

PlayerListWidget::~PlayerListWidget()
{
    delete ui;
}

void PlayerListWidget::changeEvent(QEvent *e)
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

void PlayerListWidget::onPlayerAdded(Player p)
{
    this->playerTable->addDatum(p);
}

void PlayerListWidget::onPlayersChanged(std::vector<Player> players)
{
    this->playerTable->setData(players);
}


void PlayerListWidget::onRegOpenChanged(bool regOpen)
{
    if (regOpen) {
        ui->addPlayer->setText(tr("Add Player"));
    } else {
        ui->addPlayer->setText(tr("Allow Player Registration to Add Players"));
    }
    ui->addPlayer->setDisabled(!regOpen);
}

void PlayerListWidget::onPlayerSelected(const QItemSelection &selected, const QItemSelection deselected)
{
    QModelIndexList indexes = selected.indexes();
    if (indexes.size() > 0) {
        int index = indexes[0].row();
        Player plyr = this->playerTable->getDataAt(index);
        emit this->playerSelected(plyr);
    }
}

void PlayerListWidget::addPlayerToTourn(std::string name)
{
    bool s;
    this->tourn->addPlayer(name, &s);

    if (!s) {
        QMessageBox err;
        err.setWindowTitle(tr("Cannot add player ") + QString::fromStdString(name));
        err.setText(tr("Unable to add the player due to an internal error."));
        err.exec();
    }

    // onAddPlayer is invoked by this->tourn
}

void PlayerListWidget::addPlayerClicked()
{
    AddPlayerDialogue dlg = AddPlayerDialogue(this);
    connect(&dlg, &AddPlayerDialogue::onAddPlayer, this, &PlayerListWidget::addPlayerToTourn);
    dlg.exec();
}
