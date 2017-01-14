#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include <QtCore>
#include <QDebug>
#include "QFileDialog"
#include "QFile"
#include "QTextStream"
#include "QMessageBox"
#include "decoder.h"
#include "serialdriver.h"
#include "prefsdialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void updateValLabel(const double X, const double Y, const double Z);
    void askForSerial();

private slots:
    void on_readTextButton_clicked();

    void on_reversedXChb_toggled(bool checked);

    void on_reversedYChb_toggled(bool checked);

    void on_reversedZChb_toggled(bool checked);

    void on_actionOpen_G_Code_triggered();

    void on_actionSave_G_Code_as_triggered();

    void on_actionSave_G_Code_triggered();

    void on_actionPreferences_triggered();

signals:
    void toDecode(const QStringList lines);

private:
    Ui::MainWindow *ui;
    Decoder *myDecoder;
    SerialDriver *mySerDrv;
    PrefsDialog *myPrefsDial;
    QString lastfilename;

protected:
};

#endif // MAINWINDOW_H
