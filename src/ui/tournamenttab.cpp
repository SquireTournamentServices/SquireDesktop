#include "./tournamenttab.h"
#include "./ui_tournamenttab.h"
#include "../config.h"

TournamentTab::TournamentTab(Tournament *tourn, QWidget *parent) :
    AbstractTabWidget(parent),
    ui(new Ui::TournamentTab)
{
    ui->setupUi(this);
    this->tourn = tourn;

    // Set tables up
    this->playerTableLayout = new QVBoxLayout(ui->playerTable);
    this->playerTableLayout->setAlignment(Qt::AlignTop);

    std::vector<Player> players; // Emit all props will populate the table
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

    this->tourn->emitAllProps();
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

