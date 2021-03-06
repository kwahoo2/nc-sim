#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    myDecoder = new Decoder(this);
    mySerDrv = new SerialDriver(this);
    myPrefsDial = new PrefsDialog(this); //dialog with options
    myPrefsDial->setModal(false);
    QObject::connect(this, SIGNAL(toDecode(QStringList)),
                     myDecoder, SLOT(decodeCommands(const QStringList)));
    QObject::connect(myDecoder,SIGNAL(currentPos(double,double,double)),
                     this, SLOT(updateValLabel(double,double,double)));
    QObject::connect(mySerDrv, SIGNAL(listPorts(QList<QSerialPortInfo>)),
                     myPrefsDial, SLOT(addPortsNames(QList<QSerialPortInfo>)));
    QObject::connect(myPrefsDial, SIGNAL(givePortSelection(QString)),
                     mySerDrv, SLOT(setPort(QString)));
    QObject::connect(mySerDrv, SIGNAL(askForSerial()),
                     this, SLOT(askForSerial()));

    ui->setupUi(this);

    QObject::connect(myPrefsDial, SIGNAL(setXrev(bool)),
                     this->ui->reversedXChb, SLOT(setChecked(bool)));
    QObject::connect(myPrefsDial, SIGNAL(setXrev(bool)),
                     myDecoder, SLOT(setXrev(bool)));
    QObject::connect(myPrefsDial, SIGNAL(setYrev(bool)),
                     this->ui->reversedYChb, SLOT(setChecked(bool)));
    QObject::connect(myPrefsDial, SIGNAL(setYrev(bool)),
                     myDecoder, SLOT(setYrev(bool)));
    QObject::connect(myPrefsDial, SIGNAL(setZrev(bool)),
                     this->ui->reversedZChb, SLOT(setChecked(bool)));
    QObject::connect(myPrefsDial, SIGNAL(setZrev(bool)),
                     myDecoder, SLOT(setZrev(bool)));
    QObject::connect(myPrefsDial, SIGNAL(setRelMov(bool)),
                     this->ui->relativeChb, SLOT(setChecked(bool)));

    QObject::connect(myPrefsDial, SIGNAL(setSteps(double)),
                     myDecoder, SLOT(setSteps(double)));
    QObject::connect(myPrefsDial, SIGNAL(setDefFeedSpeed(double)),
                     myDecoder, SLOT(setDefFeedSpeed(double)));
    QObject::connect(myPrefsDial, SIGNAL(setDefRapidSpeed(double)),
                     myDecoder, SLOT(setDefRapidSpeed(double)));
    QObject::connect(myPrefsDial, SIGNAL(setInvSpeed(double)),
                     myDecoder, SLOT(setInvSpeed(double)));

    lastfilename = "";

    mySerDrv->refreshPorts();
    myPrefsDial->loadOtherSettings();
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

    if (!(mySerDrv->isOpened()))
    {
        QMessageBox::information(this, tr("Error"), tr("Unable to open serial port ") +mySerDrv->getPort());
        return;
    }

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
    myDecoder->setXrev(checked);
    myPrefsDial->storeXrev(checked);
}

void MainWindow::on_reversedYChb_toggled(bool checked)
{
    myDecoder->setYrev(checked);
    myPrefsDial->storeYrev(checked);
}

void MainWindow::on_reversedZChb_toggled(bool checked)
{
    myDecoder->setZrev(checked);
    myPrefsDial->storeZrev(checked);
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

void MainWindow::on_actionPreferences_triggered()
{
    myPrefsDial->show();
}

void MainWindow::askForSerial()
{
    QMessageBox::information(this, tr("Wait"), tr("Please connect a serial device"));
    mySerDrv->refreshPorts();
}

void MainWindow::on_relativeChb_toggled(bool checked)
{
    myPrefsDial->storeRelative(checked);
}
