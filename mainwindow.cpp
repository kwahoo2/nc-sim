#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QFileDialog"
#include "QFile"
#include "QTextStream"
#include "QMessageBox"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    myDecoder = new Decoder(this);
    mySerDrv = new SerialDriver(this);
    QObject::connect(this, SIGNAL(toDecode(QStringList)),
                     myDecoder, SLOT(decodeCommands(const QStringList)));
    QObject::connect(myDecoder,SIGNAL(currentPos(double,double,double)),
                     this, SLOT(updateValLabel(double,double,double)));
    ui->setupUi(this);

    lastfilename = "";

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

        QObject::disconnect(mySerDrv, SIGNAL(recSerial(QByteArray)),
                         myDecoder, SLOT(decodeRecPos(QByteArray)));
    }

    myDecoder->resetBuffs();
    if (ui->relativeChb->isChecked()) myDecoder->resetXYZ(); //run relative position to the previous run

    QString plainTextEditContents = ui->gcodeTextEdit->toPlainText();
    plainTextEditContents = plainTextEditContents.simplified(); //clear qstring from non-necessary whitespaces
    QStringList lines = plainTextEditContents.split("\n");

    emit toDecode(lines);

    mySerDrv->openSerial();
    QObject::connect(mySerDrv, SIGNAL(getRecLen(int)),
                     myDecoder, SLOT(incrRecCounter(const int)));

    QObject::connect(myDecoder, SIGNAL(sendSingleByte(quint8)),
                     mySerDrv, SLOT(sendByte(quint8)));

    QObject::connect(mySerDrv, SIGNAL(recSerial(QByteArray)),
                     myDecoder, SLOT(decodeRecPos(QByteArray)));

    myDecoder->incrRecCounter(0); //initial batch
}

void MainWindow::updateValLabel(const double X, const double Y, const double Z)
{

    ui->labelX->setText("X: "+QString::number(X, 'f', 2));
    ui->labelY->setText("Y: "+QString::number(Y, 'f', 2));
    ui->labelZ->setText("Z: "+QString::number(Z, 'f', 2));
}

void MainWindow::on_reversedXChb_toggled(bool checked)
{
    myDecoder->reversedX = checked;
}

void MainWindow::on_reversedYChb_toggled(bool checked)
{
    myDecoder->reversedY = checked;
}

void MainWindow::on_reversedZChb_toggled(bool checked)
{
    myDecoder->reversedZ = checked;
}

void MainWindow::on_actionOpen_G_Code_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open G-Code File"));
    QFile file(QFileInfo(filename).absoluteFilePath());
    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::information(this, tr("Unable to open file"), file.errorString());
        return;
    }
    QTextStream in(&file);
    ui->gcodeTextEdit->document()->setPlainText(in.readAll());
    file.close();
}

void MainWindow::on_actionSave_G_Code_as_triggered()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Save G-Code File"));
    lastfilename = filename;
    QFile file(QFileInfo(filename).absoluteFilePath());
    if (!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::information(this, tr("Unable to open file"), file.errorString());
        return;
    }
    QTextStream out(&file);
    out << ui->gcodeTextEdit->toPlainText();
    file.close();
    ui->actionSave_G_Code->setEnabled(true);
}

void MainWindow::on_actionSave_G_Code_triggered()
{
    QFile file(QFileInfo(lastfilename).absoluteFilePath());
    if (!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::information(this, tr("Unable to open file"), file.errorString());
        return;
    }
    QTextStream out(&file);
    out << ui->gcodeTextEdit->toPlainText();
    file.close();
}
