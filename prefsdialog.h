#ifndef PREFSDIALOG_H
#define PREFSDIALOG_H

#include <QDialog>
#include <QDebug>
#include <QSerialPortInfo>
#include <QSettings>

namespace Ui {
class PrefsDialog;
}

class PrefsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PrefsDialog(QWidget *parent = 0);
    ~PrefsDialog();

public slots:
    void addPortsNames(const QList <QSerialPortInfo> ports);
    void storeXrev(const bool val);
    void storeYrev(const bool val);
    void storeZrev(const bool val);
    void storeRelative(const bool val);
    void loadOtherSettings();

private slots:
    void on_portsComboBox_activated(const QString &arg1);

    void on_stepsSpinBox_valueChanged(double arg1);

    void on_rapidSpinBox_valueChanged(double arg1);

    void on_feedSpinBox_valueChanged(double arg1);

    void on_invSpeedSpinBox_valueChanged(double arg1);

signals:
    void givePortSelection(const QString portsel);
    void setXrev(const bool val);
    void setYrev(const bool val);
    void setZrev(const bool val);
    void setRelMov(const bool val);
    void setSteps(const double val);
    void setDefFeedSpeed(const double val);
    void setDefRapidSpeed(const double val);
    void setInvSpeed(const double val);

private:
    Ui::PrefsDialog *ui;
    QSettings settings;
    QString storedport;
    void loadPortSettings();
};

#endif // PREFSDIALOG_H
