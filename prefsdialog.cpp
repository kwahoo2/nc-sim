#include "prefsdialog.h"
#include "ui_prefsdialog.h"

PrefsDialog::PrefsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PrefsDialog)
{
    ui->setupUi(this);
}

void PrefsDialog::addPortName(QString port)
{
    ui->portsComboBox->addItem(port);
}

PrefsDialog::~PrefsDialog()
{
    delete ui;
}

void PrefsDialog::on_portsComboBox_activated(const QString &arg1)
{
    emit givePortSelection(arg1);
}

