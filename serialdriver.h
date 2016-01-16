#ifndef SERIALDRIVER_H
#define SERIALDRIVER_H
#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>

class SerialDriver : public QObject
{
    Q_OBJECT
public:
    explicit SerialDriver(QObject *parent = 0);
    bool isOpened();
    QList <QSerialPortInfo> ports;

protected:

signals:
    void getRecLen(const int recLen);

private:
    QSerialPort *serial;
    QByteArray serialBuffer;
    ~SerialDriver();

private slots:
    void readSerial();

public slots:
    void sendByte(const quint8);
    void openSerial();
    void closeSerial();
    void refreshPorts();
    void setPort(const int val);
};

#endif // SERIALDRIVER_H