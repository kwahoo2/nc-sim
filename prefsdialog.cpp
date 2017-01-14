#include "prefsdialog.h"
#include "ui_prefsdialog.h"

PrefsDialog::PrefsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PrefsDialog)
{
    ui->setupUi(this);
}

PrefsDialog::~PrefsDialog()
{
    delete ui;
}

void PrefsDialog::addPortsNames(QList <QSerialPortInfo> ports)
{
    ui->portsComboBox->clear();
    int idx = 0;

    for (QSerialPortInfo &port : ports)
    {
        QString portname = port.systemLocation();
        ui->portsComboBox->addItem(portname);
        ui->portsComboBox->setItemData(idx, port.manufacturer(), Qt::ToolTipRole);
        idx++;
    }
    emit givePortSelection(ui->portsComboBox->itemData(0, Qt::DisplayRole).toString());
    loadPortSettings();
}

void PrefsDialog::on_portsComboBox_activated(const QString &arg1)
{
    settings.setValue("Port/port", arg1);
    emit givePortSelection(arg1);
}

void PrefsDialog::loadPortSettings()
{
    if (settings.contains("Port/port"))
    {
        storedport = settings.value("Port/port").toString();
        int sel = ui->portsComboBox->findData(storedport, Qt::DisplayRole);
        if (sel != -1)
        { // -1 for not found
           ui->portsComboBox->setCurrentIndex(sel);
           emit givePortSelection(storedport);
        }
    }
}
