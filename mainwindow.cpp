#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    myDecoder = new Decoder(this);
    mySerDrv = new SerialDriver(this);
    QObject::connect(this, SIGNAL(toDecode(QStringList)),
                     myDecoder, SLOT(decodeCommands(const QStringList)));
    ui->setupUi(this);

}

MainWindow::~MainWindow()
{
    mySerDrv->closeSerial();
    delete ui;
}

void MainWindow::on_readTextButton_clicked()
{
    if (mySerDrv->isOpened())
    {
        mySerDrv->closeSerial();
        QObject::disconnect(mySerDrv, SIGNAL(getRecLen(int)),
                         myDecoder, SLOT(incrRecCounter(const int)));

        QObject::disconnect(myDecoder, SIGNAL(sendSingleByte(quint8)),
                         mySerDrv, SLOT(sendByte(quint8)));
    }

    QString plainTextEditContents = ui->plainTextEdit->toPlainText();
    plainTextEditContents = plainTextEditContents.simplified(); //clear qstring from non-necessary whitespaces
    QStringList lines = plainTextEditContents.split("\n");
    myDecoder->resetBuffs();
    emit toDecode(lines);
    mySerDrv->openSerial();
    QObject::connect(mySerDrv, SIGNAL(getRecLen(int)),
                     myDecoder, SLOT(incrRecCounter(const int)));

    QObject::connect(myDecoder, SIGNAL(sendSingleByte(quint8)),
                     mySerDrv, SLOT(sendByte(quint8)));
    myDecoder->incrRecCounter(0); //initial batch

}

void MainWindow::on_xPButton_pressed()
{
    if (!mySerDrv->isOpened()) mySerDrv->openSerial();
    //qDebug() << "X+ " << endl;
    for(int i=0; i<10; i++) mySerDrv->sendByte(0b00000001);
}

void MainWindow::on_xMButton_pressed()
{
    if (!mySerDrv->isOpened()) mySerDrv->openSerial();
    //qDebug() << "X- " << endl;
    for(int i=0; i<10; i++) mySerDrv->sendByte(0b00001000);
}

void MainWindow::on_yPButton_pressed()
{
    if (!mySerDrv->isOpened()) mySerDrv->openSerial();
    //qDebug() << "Y+ " << endl;
    for(int i=0; i<10; i++) mySerDrv->sendByte(0b00000010);
}

void MainWindow::on_yMButton_pressed()
{
    if (!mySerDrv->isOpened()) mySerDrv->openSerial();
    //qDebug() << "Y- " << endl;
    for(int i=0; i<10; i++) mySerDrv->sendByte(0b00010000);
}

void MainWindow::on_zPButton_pressed()
{
    if (!mySerDrv->isOpened()) mySerDrv->openSerial();
    //qDebug() << "Z+ " << endl;
    for(int i=0; i<10; i++) mySerDrv->sendByte(0b00000100);
}

void MainWindow::on_zMButton_pressed()
{
    if (!mySerDrv->isOpened()) mySerDrv->openSerial();
    //qDebug() << "Z- " << endl;
    for(int i=0; i<10; i++) mySerDrv->sendByte(0b00100000);
}
