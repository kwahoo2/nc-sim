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
    bool reversedX, reversedY, reversedZ;

signals:
    void sendSingleByte(const quint8 byte);
    void currentPos(const double X, const double Y, const double Z);

public slots:
    void decodeCommands(const QStringList lines);
    void resetBuffs();
    void resetXYZ();
    void incrRecCounter(const int val);
    void decodeRecPos(const QByteArray recP);

private:
    void decodeMovement(const int num,
                        const std::vector <QChar> args, const std::vector <double> vals);
    double Xold, Yold, Zold;
    double Xrec, Yrec, Zrec;

    double stepsPerMM;
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
    void exportData(int X, int Y, int Z);
    void rapidMode();
    void feedMode();
    void powerDown();
    bool useIJ, useIK, useJK;
    double invSpeedconst, rapidSpeed, feedSpeed;
};

#endif // DECODER_H
