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

/*void SerialDriver::checkBuadRates()
{
    QList baudrates = QSerialPortInfo::standardBaudRates();
}*/

void SerialDriver::setPort(const QString portsel)
{
    serial->setPortName(portsel);
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

