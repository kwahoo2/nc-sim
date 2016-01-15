#ifndef DECODER_H
#define DECODER_H

#include <QObject>
#include <QtCore>
#include <QDebug>
#include <QChar>
#include <QTextStream>
#include <QFile>
#include <cmath>
#include <ctgmath>


class Decoder : public QObject
{
    Q_OBJECT
public:
    explicit Decoder(QObject *parent = 0);

signals:
    void sendSingleByte(const quint8 byte);

public slots:
    void decodeCommands(const QStringList lines);
    void resetBuffs();
    void incrRecCounter(const int val);

private:
    void decodeMovement(const QChar cmd, const int num,
                        const std::vector <QChar> args, const std::vector <double> vals);
    double Xold, Yold, Zold;
    int stepsPerMM;
    int byteCounter;
    int recCounter; //counter for received/finished byte, should be smaller than send bytes
    QString posVals;
    QTextStream posValues;
    QByteArray steps;
    std::vector <int> posValNum;

    void renderLine(double X0, double Y0, double Z0,
               double X1, double Y1, double Z1);
    void renderCircleXY(double X0, double Y0, double Z0,
                        double X1, double Y1,
                        double CX, double CY,
                        double R, bool ccw);
    void writePosFile();
    void writeStepsFile();
    void getBytes();
    void exportData(double X, double Y, double Z);
    void rapidMode();
    void feedMode();
    bool useIJ, useIK, useJK;
};

#endif // DECODER_H
