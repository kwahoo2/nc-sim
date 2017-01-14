#include "prefsdialog.h"
#include "ui_prefsdialog.h"

PrefsDialog::PrefsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PrefsDialog)
{
    ui->setupUi(this);

    QObject::connect(this, SIGNAL(setSteps(double)),
                     this->ui->stepsSpinBox, SLOT(setValue(double)));
    QObject::connect(this, SIGNAL(setDefRapidSpeed(double)),
                     this->ui->rapidSpinBox, SLOT(setValue(double)));
    QObject::connect(this, SIGNAL(setDefFeedSpeed(double)),
                     this->ui->feedSpinBox, SLOT(setValue(double)));
    QObject::connect(this, SIGNAL(setInvSpeed(double)),
                     this->ui->invSpeedSpinBox, SLOT(setValue(double)));
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

void PrefsDialog::loadOtherSettings()
{
    if (settings.contains("Reversed_Axis/xrev"))
    {
        emit setXrev(settings.value("Reversed_Axis/xrev").toBool());
    }
    if (settings.contains("Reversed_Axis/yrev"))
    {
        emit setYrev(settings.value("Reversed_Axis/yrev").toBool());
    }
    if (settings.contains("Reversed_Axis/xrev"))
    {
        emit setZrev(settings.value("Reversed_Axis/zrev").toBool());
    }
    if (settings.contains("Relative_movement/relmov"))
    {
        emit setRelMov(settings.value("Relative_movement/relmov").toBool());
    }
    if (settings.contains("Driver/stepspermm"))
    {
        emit setSteps(settings.value("Driver/stepspermm").toDouble());
    }
    if (settings.contains("Driver/rapidspeed"))
    {
        emit setDefRapidSpeed(settings.value("Driver/rapidspeed").toDouble());
    }
    if (settings.contains("Driver/feedspeed"))
    {
        emit setDefFeedSpeed(settings.value("Driver/feedspeed").toDouble());
    }
    if (settings.contains("Driver/invspeedconst"))
    {
        emit setInvSpeed(settings.value("Driver/invspeedconst").toDouble());
    }
}

void PrefsDialog::storeXrev(const bool val)
{
    settings.setValue("Reversed_Axis/xrev", val);
}

void PrefsDialog::storeYrev(const bool val)
{
    settings.setValue("Reversed_Axis/yrev", val);
}

void PrefsDialog::storeZrev(const bool val)
{
    settings.setValue("Reversed_Axis/zrev", val);
}

void PrefsDialog::storeRelative(const bool val)
{
    settings.setValue("Relative_movement/relmov", val);
}

void PrefsDialog::on_stepsSpinBox_valueChanged(double arg1)
{
    emit setSteps(arg1);
    settings.setValue("Driver/stepspermm", arg1);
}


void PrefsDialog::on_rapidSpinBox_valueChanged(double arg1)
{
    emit setDefRapidSpeed(arg1);
    settings.setValue("Driver/rapidspeed", arg1);
}

void PrefsDialog::on_feedSpinBox_valueChanged(double arg1)
{
    emit setDefFeedSpeed(arg1);
    settings.setValue("Driver/feedspeed", arg1);
}

void PrefsDialog::on_invSpeedSpinBox_valueChanged(double arg1)
{
    emit setInvSpeed(arg1);
    settings.setValue("Driver/invspeedconst", arg1);
}
