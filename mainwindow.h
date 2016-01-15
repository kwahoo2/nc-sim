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

signals:
    void toDecode(const QStringList lines);

private:
    Ui::MainWindow *ui;
    Decoder *myDecoder;
    SerialDriver *mySerDrv;

protected:
};

#endif // MAINWINDOW_H
