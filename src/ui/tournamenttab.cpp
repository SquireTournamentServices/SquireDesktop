#include "./tournamenttab.h"
#include "./ui_tournamenttab.h"
#include "./tournament/addplayerdialogue.h"
#include "./tournament/tournamentchangesettingsdialogue.h"
#include "./tournament/tournamentunsavederrordialogue.h"
#include "./tournament/standingsboardwidget.h"
#include "../config.h"
#include <QDialogButtonBox>
#include <QMessageBox>

TournamentTab::TournamentTab(Tournament *tourn, QWidget *parent) :
    AbstractTabWidget(parent),
    ui(new Ui::TournamentTab)
{
    ui->setupUi(this);
    this->tourn = tourn;
    if (!this->tourn->isSaved()) {
        this->tourn->save();
    }

    // Add player table
    this->playerTableLayout = new QVBoxLayout(ui->playerTable);
    this->playerTableLayout->setAlignment(Qt::AlignTop);

    std::vector<Player> players = this->tourn->players();
    this->playerTable = new SearchSortTableWidget<PlayerModel, Player>(players);

    QString showActivePlayers = tr("Only Show Active Players");
    this->playerTable->addAdditionalFilter(showActivePlayers.toStdString(), &playerIsActive);
    this->playerTableLayout->addWidget(playerTable);

    // Add round table
    this->roundTableLayout = new QVBoxLayout(ui->roundTable);
    this->roundTableLayout->setAlignment(Qt::AlignTop);

    std::vector<Round> rounds = this->tourn->rounds();
    this->roundTable = new SearchSortTableWidget<RoundModel, Round>(rounds);

    QString showLiveRounds = tr("Only Show Active Rounds");
    this->roundTable->addAdditionalFilter(showLiveRounds.toStdString(), &roundIsActive);
    this->roundTableLayout->addWidget(this->roundTable);

    // Add information widgets
    this->roundViewWidget = new RoundViewWidget(this->tourn, this);
    ui->infoTabWidget->addTab(this->roundViewWidget, tr("Match Info"));

    this->playerViewWidget = new PlayerViewWidget(this->tourn, this);
    ui->infoTabWidget->addTab(this->playerViewWidget, tr("Player Info"));

    connect(this->roundViewWidget, &RoundViewWidget::playerSelected, this->playerViewWidget, &PlayerViewWidget::setPlayer);
    connect(this->playerViewWidget, &PlayerViewWidget::roundSelected, this->roundViewWidget, &RoundViewWidget::setRound);

    // Connect all slots
    connect(this->tourn, &Tournament::onPlayerAdded, this, &TournamentTab::onPlayerAdded);
    connect(this->tourn, &Tournament::onPlayersChanged, this, &TournamentTab::onPlayersChanged);
    connect(this->tourn, &Tournament::onRoundAdded, this, &TournamentTab::onRoundAdded);
    connect(this->tourn, &Tournament::onRoundsChanged, this, &TournamentTab::onRoundsChanged);
    connect(this->tourn, &Tournament::onNameChanged, this, &TournamentTab::onNameChanged);
    connect(this->tourn, &Tournament::onUseTableNumberChanged, this, &TournamentTab::onUseTableNumberChanged);
    connect(this->tourn, &Tournament::onFormatChanged, this, &TournamentTab::onFormatChanged);
    connect(this->tourn, &Tournament::onMinDeckCountChanged, this, &TournamentTab::onMinDeckCountChanged);
    connect(this->tourn, &Tournament::onMaxDeckCountChanged, this, &TournamentTab::onMaxDeckCountChanged);
    connect(this->tourn, &Tournament::onPairingTypeChanged, this, &TournamentTab::onPairingTypeChanged);
    connect(this->tourn, &Tournament::onSaveLocationChanged, this, &TournamentTab::onSaveLocationChanged);
    connect(this->tourn, &Tournament::onStatusChanged, this, &TournamentTab::onStatusChanged);
    connect(this->tourn, &Tournament::onRegOpenChanged, this, &TournamentTab::onRegOpenChanged);
    connect(this->tourn, &Tournament::onSaveStatusChanged, this, &TournamentTab::onSaveStatusChanged);

    this->tourn->emitAllProps();

    // Connect GUI
    connect(ui->pairRound, &QPushButton::clicked, this, &TournamentTab::pairRoundsClicked);
    connect(ui->addPlayer, &QPushButton::clicked, this, &TournamentTab::addPlayerClicked);
    connect(ui->tournamentSettings, &QPushButton::clicked, this, &TournamentTab::changeSettingsClicked);
    connect(this->roundTable->selectionModel(), &QItemSelectionModel::selectionChanged, this, &TournamentTab::roundSelected);
    connect(this->playerTable->selectionModel(), &QItemSelectionModel::selectionChanged, this, &TournamentTab::playerSelected);

    // Add menu
    QMenu *tournamentsMenu = this->addMenu(tr("Tournament"));
    QAction *addPlayerAction = tournamentsMenu->addAction(tr("Add player"));
    connect(addPlayerAction, &QAction::triggered, this, &TournamentTab::addPlayerClicked);

    QAction *pairRoundsAction = tournamentsMenu->addAction(tr("Pair Round"));
    connect(pairRoundsAction, &QAction::triggered, this, &TournamentTab::pairRoundsClicked);

    QAction *changeSettingsAction = tournamentsMenu->addAction(tr("Change Settings"));
    connect(changeSettingsAction, &QAction::triggered, this, &TournamentTab::changeSettingsClicked);

    QAction *closeTournamentAction = tournamentsMenu->addAction(tr("Close Tournament"));
    connect(closeTournamentAction, &QAction::triggered, this, &TournamentTab::closeTab);

    QAction *confirmAllMatchesAction = tournamentsMenu->addAction(tr("Confirm All Matches"));
    connect(confirmAllMatchesAction, &QAction::triggered, this, &TournamentTab::confirmAllMatches);

    QAction *showStandingsAction = tournamentsMenu->addAction(tr("Show Standings"));
    connect(showStandingsAction, &QAction::triggered, this, &TournamentTab::showStandings);

    // Start timer
    connect(&this->timeLeftUpdater, &QTimer::timeout, this, &TournamentTab::updateRoundTimer);
    this->updateRoundTimer();
}

TournamentTab::~TournamentTab()
{
    delete ui;
    delete playerTable;
    delete playerTableLayout;
    delete roundTable;
    delete roundTableLayout;
    delete tourn;
    delete roundViewWidget;
    delete playerViewWidget;
}

void TournamentTab::changeEvent(QEvent *e)
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

void TournamentTab::closeTab()
{
    emit this->close();
}

void TournamentTab::setStatus()
{
    std::string status = this->t_name + " - " + this->t_format + " - " + this->t_type;
    ui->tournamentStatus->setText(QString::fromStdString(status));
}

bool TournamentTab::canExit()
{
    bool canExit = false;
    if (this->tourn->isSaved()) {
        canExit = true;
    } else {
        TournamentUnsavedErrorDialogue dlg = TournamentUnsavedErrorDialogue(this->tourn);
        return dlg.exec() == 1;
    }

    if (canExit) {
        this->tourn->close();
        return true;
    } else {
        return false;
    }
}

void TournamentTab::onPlayerAdded(Player p)
{
    this->playerTable->addDatum(p);
}

void TournamentTab::onPlayersChanged(std::vector<Player> players)
{
    this->playerTable->setData(this->tourn->players());
}

void TournamentTab::onRoundAdded(Round r)
{
    this->roundTable->addDatum(r);
    updateRoundTimer();
}

void TournamentTab::onRoundsChanged(std::vector<Round> rounds)
{
    this->roundTable->setData(rounds);
    this->roundViewWidget->rerender();
    updateRoundTimer();
}

void TournamentTab::onNameChanged(std::string str)
{
    this->t_name = str;
    this->setStatus();
}

void TournamentTab::onUseTableNumberChanged(bool utn)
{

}

void TournamentTab::onFormatChanged(std::string str)
{
    this->t_format = str;
    this->setStatus();
}

void TournamentTab::onMinDeckCountChanged(int mdc)
{

}

void TournamentTab::onMaxDeckCountChanged(int mdc)
{

}

void TournamentTab::onPairingTypeChanged(squire_core::sc_TournamentPreset type)
{
    switch(type) {
    case squire_core::sc_TournamentPreset::Fluid:
        this->t_type = tr("Fluid").toStdString();
        break;
    case squire_core::sc_TournamentPreset::Swiss:
        this->t_type = tr("Swiss").toStdString();
        break;
    }
    this->setStatus();
}

void TournamentTab::onSaveLocationChanged(std::string str)
{

}

void TournamentTab::onStatusChanged(squire_core::sc_TournamentStatus status)
{
    if (status == squire_core::sc_TournamentStatus::Started) {
        ui->startedIndicator->setChecked(true);
        ui->startedIndicator->setText(tr("Started"));
    } else {
        ui->startedIndicator->setChecked(false);
        ui->startedIndicator->setText(tr("Not Started"));
    }

    bool canPairRounds = false;
    switch(status) {
    case squire_core::sc_TournamentStatus::Planned:
        ui->statusIndicator->setText(tr("Planned"));
        break;
    case squire_core::sc_TournamentStatus::Started:
        ui->statusIndicator->setText(tr("Started"));
        canPairRounds = true;
        break;
    case squire_core::sc_TournamentStatus::Frozen:
        ui->statusIndicator->setText(tr("Frozen"));
        break;
    case squire_core::sc_TournamentStatus::Ended:
        ui->statusIndicator->setText(tr("Ended"));
        break;
    case squire_core::sc_TournamentStatus::Cancelled:
        ui->statusIndicator->setText(tr("Cancelled"));
        break;
    }

    if (!this->tourn->isSaved()) {
        ui->statusIndicator->setText(ui->statusIndicator->text() + tr("* (Unsaved Changes)"));
    }
    ui->pairRound->setDisabled(!canPairRounds);
}

void TournamentTab::addPlayerToTourn(std::string name)
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

void TournamentTab::addPlayerClicked()
{
    AddPlayerDialogue dlg = AddPlayerDialogue(this);
    connect(&dlg, &AddPlayerDialogue::onAddPlayer, this, &TournamentTab::addPlayerToTourn);
    dlg.exec();
}

void TournamentTab::updateRoundTimer()
{
    int roundCount = 0;
    long max = 0;
    long min = -1;
    std::vector<Round> rounds = this->tourn->rounds();
    this->roundTable->refreshTable();

    for (Round r : rounds) {
        long tl = r.time_left();
        if (tl < min || min == -1) {
            min = tl;
        }

        long d = r.duration();
        if (d > max) {
            max = d;
        }

        if (roundIsActive(r)) {
            roundCount++;
        }
    }
    if (min == -1) {
        min = 0;
    }

    QString str = "";
    if (min == 0) {
        str = tr("No Time");
    } else {
        int seconds = min % 60;
        int minutes = ((min / 60) % 60);
        int hours = min / (60 * 60);

        if (hours < 10) {
            str += "0";
        }
        str += QString::number(hours);
        str += ":";

        if (minutes < 10) {
            str += "0";
        }
        str += QString::number(minutes);
        str += ":";

        if (seconds < 10) {
            str += "0";
        }
        str += QString::number(seconds);
    }

    str += " " + tr("Left in Round");
    str += tr(" (") + QString::number(roundCount, 10) + tr(" Active Matches)");

    ui->roundTimerLabel->setText(str);
    if (max == 0) {
        ui->progressBar->setValue(0);
    } else {
        ui->progressBar->setValue((100 * min) / max);
    }
    this->timeLeftUpdater.start(100);
}

void TournamentTab::pairRoundsClicked()
{
    this->tourn->pairRounds();

    // onAddRound is invoked by tourn
}

void TournamentTab::changeSettingsClicked()
{
    TournamentChangeSettingsDialogue dlg = TournamentChangeSettingsDialogue(this->tourn, this);
    dlg.exec();
}

void TournamentTab::onSaveStatusChanged(bool status)
{
    this->onStatusChanged(this->tourn->status());
}

void TournamentTab::onRegOpenChanged(bool regOpen)
{
    if (regOpen) {
        ui->addPlayer->setText(tr("Add Player"));
    } else {
        ui->addPlayer->setText(tr("Allow Player Registration to Add Players"));
    }
    ui->addPlayer->setDisabled(!regOpen);
}

void TournamentTab::roundSelected(const QItemSelection &selected, const QItemSelection deselected)
{
    QModelIndexList indexes = selected.indexes();
    if (indexes.size() > 0) {
        int index = indexes[0].row();
        Round rnd = this->roundTable->getDataAt(index);
        this->roundViewWidget->setRound(rnd);
    }
}

void TournamentTab::playerSelected(const QItemSelection &selected, const QItemSelection deselected)
{
    QModelIndexList indexes = selected.indexes();
    if (indexes.size() > 0) {
        int index = indexes[0].row();
        Player plyr = this->playerTable->getDataAt(index);
        this->playerViewWidget->setPlayer(plyr);
    }
}

void TournamentTab::confirmAllMatches()
{
    for (Round r : this->tourn->rounds()) {
        for (Player p : r.players()) {
            bool res = this->tourn->confirmPlayer(r, p);
            if (!res) {
                QMessageBox msg;
                msg.setText(tr("Cannot confirm player ") +
                            QString::fromStdString(p.all_names()) +
                            tr(" in round ") +
                            QString::number(r.match_number()));
                msg.setWindowTitle(tr("Cannot confirm all rounds."));
                msg.exec();
                return;
            }
        }
    }
}

void TournamentTab::showStandings()
{
    StandingsBoardWidget *w = new StandingsBoardWidget(this->tourn, this);
    w->show();
}
