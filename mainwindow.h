#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include <QtCore>
#include <QDebug>
#include "decoder.h"
#include "serialdriver.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_readTextButton_clicked();

    void on_xPButton_pressed();

    void on_xMButton_pressed();

    void on_yPButton_pressed();

    void on_yMButton_pressed();

    void on_zPButton_pressed();

    void on_zMButton_pressed();

signals:
    void toDecode(const QStringList lines);

private:
    int repeatdelay;
    Ui::MainWindow *ui;
    Decoder *myDecoder;
    SerialDriver *mySerDrv;

protected:
};

#endif // MAINWINDOW_H
