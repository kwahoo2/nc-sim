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

    //mySerDrv->closeSerial();
}
