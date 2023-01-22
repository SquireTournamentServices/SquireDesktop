#include "playerviewwidget.h"
#include "ui_playerviewwidget.h"
#include <QMessageBox>
#include <squire_ffi/squire_ffi.h>

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
    connect(&this->timeLeftUpdater, &QTimer::timeout, this, &PlayerViewWidget::displayTime);
    connect(this->roundTable->selectionModel(), &QItemSelectionModel::selectionChanged, this, &PlayerViewWidget::onRoundSelected);
    connect(ui->dropButton, &QPushButton::clicked, this, &PlayerViewWidget::dropPlayer);

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

QString PlayerViewWidget::getStatusString()
{
    QString base = QString::fromStdString(this->player.all_names())
                   + " - ";

    switch(this->player.status()) {
    case squire_ffi::sc_PlayerStatus::Registered:
        base += tr("Active");
        break;
    case squire_ffi::sc_PlayerStatus::Dropped:
        base += tr("Dropped");
        break;
    }

    return base;
}

void PlayerViewWidget::displayTime()
{
    this->roundTable->refreshTable();
}

void PlayerViewWidget::displayPlayer()
{
    QString status = tr("No Player Selected");
    if (this->playerSelected) {
        this->timeLeftUpdater.start(100);
        status = this->getStatusString();
        this->roundTable->setData(this->tourn->playerRounds(this->player));
    } else {
        this->roundTable->setData(std::vector<Round>());
    }

    this->displayTime();
    ui->playerStatus->setText(status);
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

void PlayerViewWidget::dropPlayer()
{
    bool s = this->tourn->dropPlayer(this->player);
    if (!s) {
        QMessageBox msg;
        msg.setWindowTitle(tr("Cannot drop player ") + QString::fromStdString(this->player.all_names()));
        msg.setText(tr("Cannot drop player ") + QString::fromStdString(this->player.all_names()));
        msg.exec();
    }
}
