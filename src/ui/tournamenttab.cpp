#include "./tournamenttab.h"
#include "./ui_tournamenttab.h"

TournamentTab::TournamentTab(Tournament tourn, QWidget *parent) :
    AbstractTabWidget(parent),
    ui(new Ui::TournamentTab)
{
    ui->setupUi(this);
    this->tourn = tourn;

    // Set tables up
    this->playerTableLayout = new QVBoxLayout(ui->playerTable);
    this->playerTableLayout->setAlignment(Qt::AlignTop);

    std::vector<Player> players = tourn.players();
    this->playerTable = new SearchSortTableWidget<AbstractPlayerModel, Player>(players);
    this->playerTableLayout->addWidget(playerTable);

    this->roundTableLayout = new QVBoxLayout(ui->roundTable);
    this->roundTableLayout->setAlignment(Qt::AlignTop);
}

TournamentTab::~TournamentTab()
{
    delete ui;
    delete playerTable;
    delete playerTableLayout;
    delete roundTableLayout;
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

bool TournamentTab::canExit()
{
    this->tourn.close();
    return true;
}

