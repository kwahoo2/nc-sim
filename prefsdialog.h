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

private slots:
    void on_portsComboBox_activated(const QString &arg1);

signals:
    void givePortSelection(const QString portsel);

private:
    Ui::PrefsDialog *ui;
    QSettings settings;
    QString storedport;
    void loadPortSettings();
};

#endif // PREFSDIALOG_H
