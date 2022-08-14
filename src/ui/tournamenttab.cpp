#include "./tournamenttab.h"
#include "./ui_tournamenttab.h"
#include "./tournament/addplayerdialogue.h"
#include "../config.h"
#include <QMessageBox>

TournamentTab::TournamentTab(Tournament *tourn, QWidget *parent) :
    AbstractTabWidget(parent),
    ui(new Ui::TournamentTab)
{
    ui->setupUi(this);
    this->tourn = tourn;

    // Set tables up
    this->playerTableLayout = new QVBoxLayout(ui->playerTable);
    this->playerTableLayout->setAlignment(Qt::AlignTop);

    std::vector<Player> players = this->tourn->players();
    this->playerTable = new SearchSortTableWidget<AbstractPlayerModel, Player>(players);
    this->playerTableLayout->addWidget(playerTable);

    this->roundTableLayout = new QVBoxLayout(ui->roundTable);
    this->roundTableLayout->setAlignment(Qt::AlignTop);

    // Connect all slots
    connect(this->tourn, &Tournament::onPlayerAdded, this, &TournamentTab::onPlayerAdded);
    connect(this->tourn, &Tournament::onPlayersChanged, this, &TournamentTab::onPlayersChanged);
    connect(this->tourn, &Tournament::onNameChanged, this, &TournamentTab::onNameChanged);
    connect(this->tourn, &Tournament::onUseTableNumberChanged, this, &TournamentTab::onUseTableNumberChanged);
    connect(this->tourn, &Tournament::onFormatChanged, this, &TournamentTab::onFormatChanged);
    connect(this->tourn, &Tournament::onMinDeckCountChanged, this, &TournamentTab::onMinDeckCountChanged);
    connect(this->tourn, &Tournament::onMaxDeckCountChanged, this, &TournamentTab::onMaxDeckCountChanged);
    connect(this->tourn, &Tournament::onPairingTypeChanged, this, &TournamentTab::onPairingTypeChanged);
    connect(this->tourn, &Tournament::onSaveLocationChanged, this, &TournamentTab::onSaveLocationChanged);
    connect(this->tourn, &Tournament::onStatusChanged, this, &TournamentTab::onStatusChanged);

    this->tourn->emitAllProps();

    // Connect GUI
    connect(ui->addPlayer, &QPushButton::clicked, this, &TournamentTab::addPlayerClicked);
}

TournamentTab::~TournamentTab()
{
    delete ui;
    delete playerTable;
    delete playerTableLayout;
    delete roundTableLayout;
    delete tourn;
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

void TournamentTab::setStatus()
{
    std::string status = this->t_name + " - " + this->t_format + " - " + this->t_type;
    ui->tournamentStatus->setText(QString::fromStdString(status));
}

bool TournamentTab::canExit()
{
    this->tourn->close();
    return true;
}

void TournamentTab::onPlayerAdded(Player p)
{
    this->playerTable->addDatum(p);
}

void TournamentTab::onPlayersChanged(std::vector<Player> players)
{
    this->playerTable->setData(this->tourn->players());
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
    } else {
        ui->startedIndicator->setChecked(false);
    }
    switch(status) {
    case squire_core::sc_TournamentStatus::Planned:
        ui->statusIndicator->setText(tr("Planned"));
        break;
    case squire_core::sc_TournamentStatus::Started:
        ui->statusIndicator->setText(tr("Started"));
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
