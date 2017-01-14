#include "serialdriver.h"

SerialDriver::SerialDriver(QObject *parent) : QObject(parent)
{
    serial = new QSerialPort(this);
    savedport = "/dev/ttyUSB0";
}
void SerialDriver::refreshPorts()
{
    QList <QSerialPortInfo> ports;
    ports = QSerialPortInfo::availablePorts();
    if (ports.isEmpty())
    {
        emit askForSerial();
    }
    else
    {
        emit listPorts(ports);
    }

}

void SerialDriver::setPort(const QString portsel)
{
    savedport = portsel;
    serial->setPortName(portsel);
    qDebug() << "Port set to " << serial->portName();
}

QString SerialDriver::getPort()
{
    return serial->portName();
}

void SerialDriver::openSerial()
{
    serial->setPortName(savedport);
    serial->setBaudRate(38400);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->open(QIODevice::ReadWrite);

    serial->clear();

    QObject::connect(serial, SIGNAL(readyRead()),
                     this, SLOT(readSerial()));
}

void SerialDriver::clearSer()
{
    serial->clear();
    serialBuffer.clear();
}

void SerialDriver::sendByte(const quint8 val)
{
    char str[] = {static_cast<char>(val)};
    serial->write(str, sizeof(str));
}

void SerialDriver::closeSerial()
{
    QObject::disconnect(serial, SIGNAL(readyRead()),
                     this, SLOT(readSerial()));
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
        emit recSerial(serialBuffer);
}


