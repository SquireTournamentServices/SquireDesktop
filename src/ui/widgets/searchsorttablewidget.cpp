#include <QAbstractButton>
#include "./searchsorttablewidget.h"

sstw_qobject::sstw_qobject(Ui::SearchSortTableWidget *ui, QWidget *parent) :
    QWidget(parent)
{
    this->addFilterBoxes = std::vector<QCheckBox *>();
}

void sstw_qobject::finishSstwSetup(Ui::SearchSortTableWidget *ui, tm_qobject *sortObject)
{
    this->ui = ui;
    this->layout = new QVBoxLayout(ui->cont);
    this->layout->setAlignment(Qt::AlignTop);

    connect(ui->searchbar, &QLineEdit::textChanged, this, &sstw_qobject::onFilterChange);
    connect(sortObject, &tm_qobject::sortChanged, this, &sstw_qobject::sortChanged);
}

sstw_qobject::~sstw_qobject()
{
    for (QCheckBox *box : this->addFilterBoxes) {
        delete box;
    }
    delete this->layout;
}

void sstw_qobject::changeEvent(QEvent *e)
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

bool sstw_qobject::isBoxSelected(int i)
{
    if (i < this->addFilterBoxes.size()) {
        return this->addFilterBoxes[i]->isChecked();
    } else {
        return false;
    }
}

void sstw_qobject::addBox(std::string boxName)
{
    QCheckBox *box = new QCheckBox(QString::fromStdString(boxName));
    this->addFilterBoxes.push_back(box);
    this->layout->addWidget(box);

    connect(box, &QAbstractButton::pressed, this, &sstw_qobject::addFilter);
}

void sstw_qobject::onFilterChange(QString query)
{

}

void sstw_qobject::addFilter()
{

}

void sstw_qobject::sortChanged(int column, bool ascending)
{

}

