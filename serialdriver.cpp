#include "serialdriver.h"

SerialDriver::SerialDriver(QObject *parent) : QObject(parent)
{
    serial = new QSerialPort(this);
    serial->setPortName("/dev/ttyUSB0");
}
void SerialDriver::refreshPorts()
{
    ports = QSerialPortInfo::availablePorts();
}

void SerialDriver::setPort(const int val)
{
    QString portName = ports[val].portName();
    serial->setPortName("/dev/"+portName); //linux only!
    qDebug() << "Port set to " << serial->portName();
}

void SerialDriver::openSerial()
{

    serial->setBaudRate(38400);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->open(QIODevice::ReadWrite);

    serial->clear();

    QObject::connect(serial, SIGNAL(readyRead()),
                     this, SLOT(readSerial()));
}

void SerialDriver::sendByte(const quint8 val)
{
    char str[] = {static_cast<char>(val)};
    serial->write(str, sizeof(str));
    //QString binStr( QString::number(val, 2 ) );
    //qDebug() << "Sended " << binStr;
}

void SerialDriver::closeSerial()
{
    serial->close();
}

bool SerialDriver::isOpened()
{
    return serial->isOpen();
}

SerialDriver::~SerialDriver()
{
    serial->close();
}


void SerialDriver::readSerial()
{
        serialBuffer = serial->readAll();
        int recLen = serialBuffer.length();
        emit getRecLen(recLen);
}

