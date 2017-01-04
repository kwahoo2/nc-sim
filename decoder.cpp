#include "decoder.h"
#define stepsBufSize 20

Decoder::Decoder(QObject *parent) : QObject(parent)
{
    Xold = 0.0;
    Yold = 0.0;
    Zold = 0.0;
    reversedX = false,
    reversedY = false,
    reversedZ = false;
    byteCounter = 0;
    recCounter = 0;
    useIJ = 1; //important for G02 and G03 commands, driven by G17, G18 and G19
    useIK = 0;
    useJK = 0;
    stepsPerMM = 96;
    invSpeedconst = 3.2; //magic number used to calculate timers
    rapidSpeed = 3.2; //mm/s
    feedSpeed = 0.32;

    posValues.setString(&posVals);
}

void Decoder::decodeCommands(const QStringList lines)
{
    std::vector <double> vals;
    std::vector <QChar> args;
    QChar cmd;
    int number = -1; //numbers fo Gxx

    for (int i = 0; i < lines.count(); i++)
    {
        QString currentLine = lines[i];
        QChar fc = currentLine.at(0); //if the first char in a new line is not G, copy old Gxx and send further
        if (fc != QLatin1Char('G'))
        {
            if (number != -1)
            {
                decodeMovement(number,
                           args, vals);
                vals.clear();
                args.clear();
            }
        }
        //qDebug() << currentLine;

        QStringList cmds = currentLine.split(" ");
        for (int j = 0; j < cmds.count(); j++)
        {
            QString currentCmd = cmds[j];
            //qDebug() << currentCmd;
            QChar c = currentCmd.at(0);

            if (c == QLatin1Char('G'))
            {
                if (number != -1) //send data from previous run
                {
                    decodeMovement(number,
                               args, vals);
                }

                vals.clear(); //preparing for a new packet
                args.clear();

                QString partial;
                for (int s = 1; s < currentCmd.count(); s++)
                {
                    partial += currentCmd.at(s);
                }
                number = partial.toInt();
                //qDebug() << number;
                cmd = c;
            }
            else if(c == QLatin1Char('X') || c == QLatin1Char('Y') || c == QLatin1Char('Z')
                    || c == QLatin1Char('I') || c == QLatin1Char('J') || c == QLatin1Char('K')
                    || c == QLatin1Char('F'))
            {
                QString partial;
                for (int s = 1; s < currentCmd.count(); s++)
                {
                    partial += currentCmd.at(s);
                }
                double value = partial.toDouble();
                //qDebug() << value;
                vals.push_back(value);
                args.push_back(c);
            }

        }
    }
    if (number != -1) //read the last line
    {
        decodeMovement(number,
                   args, vals);
    }
    writePosFile(); //write positions (int) to file
    writeStepsFile();
}
void Decoder::decodeMovement(const int num,
                    const std::vector <QChar> args, const std::vector <double> vals)
{
    /*qDebug() << cmd << num;
    for (uint i = 0; i < args.size(); i++)
    {
        qDebug() << args.at(i) << vals.at(i);
    }*/

    if (num == 17) {useIJ = 1; useIK = 0; useJK = 0;}
    if (num == 18) {useIJ = 0; useIK = 1; useJK = 0;}
    if (num == 19) {useIJ = 0; useIK = 0; useJK = 1;}

    if ((num == 0) || (num == 1)) //G00 rapid linear move and G01 feed linear move
    {
        double X = Xold;
        double Y = Yold;
        double Z = Zold;
        double F = 0.0;
        for (uint i = 0; i < args.size(); i++)
        {
            QChar c = args.at(i);
            if (c == QLatin1Char('X')) X = vals.at(i);
            if (c == QLatin1Char('Y')) Y = vals.at(i);
            if (c == QLatin1Char('Z')) Z = vals.at(i);
            if (c == QLatin1Char('F')) F = vals.at(i);
        }
        if (num == 0)
        {
            if (F!=0.0) rapidSpeed = F;
            rapidMode();
        }
        else
        {
            if (F!=0.0) feedSpeed = F;
            feedMode();
        }
        renderLine(Xold, Yold, Zold, X, Y, Z);
        Xold = X; Yold = Y; Zold = Z;
    }
    if ((num == 2) || (num == 3)) //feed cw arc and ccw arc
    {
        double X = Xold;
        double Y = Yold;
        double Z = Zold;
        double R = -1.0; //negative radius does not exist, do it can be used as flag
        double I = 0.0;
        double J = 0.0;
        double F = 0.0;
        //double K = 0.0;
        double CX, CY;
        bool ccw = 0;

        if (num == 03) ccw = 1;

        for (uint i = 0; i < args.size(); i++)
        {
            QChar c = args.at(i);

            if (c == QLatin1Char('R')) R = vals.at(i);
            if (c == QLatin1Char('I')) I = vals.at(i);
            if (c == QLatin1Char('J')) J = vals.at(i);
            if (c == QLatin1Char('X')) X = vals.at(i);
            if (c == QLatin1Char('Y')) Y = vals.at(i);
            if (c == QLatin1Char('Z')) Z = vals.at(i);
            //if (c == QLatin1Char('K')) K = vals.at(i);
            if (c == QLatin1Char('F')) F = vals.at(i); //feed velocity reading
        }

        if (F!=0.0) feedSpeed = F;
        feedMode();

        if ((useIJ) && (R < 0.0))
        {
            CX = Xold + I;
            CY = Yold + J;
            R = std::sqrt((I * I) + (J * J));
            renderCircleXY(Xold, Yold, Zold, X, Y, CX, CY, R, ccw);
        }

        Xold = X; Yold = Y; Zold = Z;
    }
}

void Decoder::renderLine(double X0, double Y0, double Z0,
           double X1, double Y1, double Z1)
{
    X0 = X0 * stepsPerMM;
    Y0 = Y0 * stepsPerMM;
    Z0 = Z0 * stepsPerMM;
    X1 = X1 * stepsPerMM;
    Y1 = Y1 * stepsPerMM;
    Z1 = Z1 * stepsPerMM;

    int Z = std::round(Z0);
    int X = std::round(X0);
    int Y = std::round(Y0);

    qDebug() << "Line XYZ " << X0 << Y0 << Z0 << X1 << Y1 << Z1;

    int Xend = std::round(X1);
    int Yend = std::round(Y1);
    int Zend = std::round(Z1);
    int Xprobe[7], Yprobe[7], Zprobe[7]; //have to test 7 (octree minus 1) points distance to line, point by point

    int dx, dy, dz;
    (X1 > X0) ? dx = 1 : dx = - 1;
    (Y1 > Y0) ? dy = 1 : dy = - 1;
    (Z1 > Z0) ? dz = 1 : dz = - 1;

    if (X1 == X0) dx = 0;
    if (Y1 == Y0) dy = 0;
    if (Z1 == Z0) dz = 0;

    while (!((X == Xend) && (Y == Yend) && (Z == Zend)))
    {
        double dist, numerator, denominator, Xtmp = 0, Ytmp = 0, Ztmp = 0;
        double olddist = 10000;

        Xprobe[0] = X + dx;
        Yprobe[0] = Y;
        Zprobe[0] = Z;

        Xprobe[1] = X;
        Yprobe[1] = Y + dy;
        Zprobe[1] = Z;

        Xprobe[2] = X;
        Yprobe[2] = Y;
        Zprobe[2] = Z + dz;

        Xprobe[3] = X + dx;
        Yprobe[3] = Y + dy;
        Zprobe[3] = Z;

        Xprobe[4] = X + dx;
        Yprobe[4] = Y;
        Zprobe[4] = Z + dz;

        Xprobe[5] = X;
        Yprobe[5] = Y + dy;
        Zprobe[5] = Z + dz;

        Xprobe[6] = X + dx;
        Yprobe[6] = Y + dy;
        Zprobe[6] = Z + dz;

        for (int i = 0; i < 7; i++)
        {
            if (((Xprobe[i] - X) != 0) || ((Yprobe[i] - Y) != 0) || ((Zprobe[i] - Z) != 0))  //avoid step = 0
            {
                double X01 = X1 - X0;
                double Y01 = Y1 - Y0;
                double Z01 = Z1 - Z0;

                double Xpb = X0 - Xprobe[i];
                double Ypb = Y0 - Yprobe[i];
                double Zpb = Z0 - Zprobe[i];

                numerator = std::abs(std::sqrt(std::pow((Y01 * Zpb - Z01 * Ypb), 2) + std::pow((X01 * Zpb - Z01 * Xpb), 2) + std::pow((X01 * Ypb - Y01 * Xpb), 2)));
                denominator = std::sqrt(std::pow(X01, 2) + std::pow(Y01, 2) + std::pow(Z01, 2));
                dist = numerator / denominator;
                    if (dist < olddist)
                    {
                        olddist = dist;
                        Xtmp = Xprobe[i];
                        Ytmp = Yprobe[i];
                        Ztmp = Zprobe[i];
                    }
            }
        }
        X = Xtmp;
        Y = Ytmp;
        Z = Ztmp;

        if (dist >= 2) //? for more testing
        {
            qDebug() << "Something went wrong. Distance is: " << dist;
            break;
        }
        //here output point, what about endpoints?
        exportData(X, Y, Z);
    }


}
void Decoder::renderCircleXY(double X0, double Y0, double Z0,
                             double X1, double Y1,
                             double CX, double CY, double R, bool ccw)
{
    X0 = X0 * stepsPerMM;
    Y0 = Y0 * stepsPerMM;
    Z0 = Z0 * stepsPerMM;
    X1 = X1 * stepsPerMM;
    Y1 = Y1 * stepsPerMM;
    CX = CX * stepsPerMM;
    CY = CY * stepsPerMM;
    R = R * stepsPerMM;


    qDebug() << "Circle XY " << X0 << Y0 << X1 << Y1 << CX << CY << R << ccw;
    int X = std::round(X0);
    int Y = std::round(Y0);
    int Xend = std::round(X1);
    int Yend = std::round(Y1);
    int Xprobe[3], Yprobe[3]; //have to test 3 points distance to circle, point by point
    bool fullCircle = false;
    int dx = 0, dy = 0;

    if ((X == Xend) && (Y == Yend)) fullCircle = true;

    while (!((X == Xend) && (Y == Yend) && (!fullCircle)))
    {
        if (ccw)
        {
            if ((X >= CX) && (Y >= CY)) //first quarter
            {
                dx = -1;
                dy = 1;
            }
            if ((X <= CX) && (Y >= CY))
            {
                dx = -1;
                dy = -1;
            }
            if ((X <= CX) && (Y <= CY))
            {
                dx = 1;
                dy = -1;
            }
            if ((X >= CX) && (Y <= CY))
            {
                dx = 1;
                dy = 1;
            }
        }
        else //CW case
        {
            if ((X >= CX) && (Y >= CY)) //first quarter
            {
                dx = 1;
                dy = -1;
            }
            if ((X <= CX) && (Y >= CY))
            {
                dx = 1;
                dy = 1;
            }
            if ((X <= CX) && (Y <= CY))
            {
                dx = -1;
                dy = 1;
            }
            if ((X >= CX) && (Y <= CY))
            {
                dx = -1;
                dy = -1;
            }
        }

        Xprobe[0] = X + dx;
        Yprobe[0] = Y;
        Xprobe[1] = X;
        Yprobe[1] = Y + dy;
        Xprobe[2] = X + dx;
        Yprobe[2] = Y + dy;

        double dist, Xtmp = 0, Ytmp = 0;
        double olddist = 10000;
        for (int i = 0; i < 3; i++)
        {
            dist = std::abs(std::sqrt(std::pow((CX - Xprobe[i]), 2) + std::pow((CY - Yprobe[i]), 2)) - R);
            if (dist < olddist)
            {
                olddist = dist;
                Xtmp = Xprobe[i];
                Ytmp = Yprobe[i];
            }
        }

        X = Xtmp;
        Y = Ytmp;

        fullCircle = false;

        //here output point
        exportData(X, Y, Z0);
    }
}

void Decoder::rapidMode()
{
    quint8 byte = 0b11000000;//0b11 reserved for velocity setup
    int timerVal = round(invSpeedconst / rapidSpeed);
    byte = byte | static_cast<quint8>(timerVal);
    steps.append(byte);
    byte = 0b10100001; //0b101 reserved for powersetup, 01 to keep motor power down
    steps.append(byte);
}

void Decoder::feedMode()
{
    quint8 byte = 0b11000000;//0b11 reserved for velocity setup,
    int timerVal = round(invSpeedconst / feedSpeed);
    byte = byte | static_cast<quint8>(timerVal);
    steps.append(byte);
    byte = 0b10100000; //0b101 reserved for powersetup, 00 steppers and motor up
    steps.append(byte);
}

void Decoder::exportData(double X, double Y, double Z)
{
    posValues << X << " " << Y << " " << Z <<"\n";
    posValNum.push_back(X);
    posValNum.push_back(Y);
    posValNum.push_back(Z);

    if (posValNum.size() > 3)
    {
        quint8 byte = 0;
        // TODO

        int Xstep = (posValNum.at(posValNum.size() - 3) - posValNum.at(posValNum.size() - 6));
        int Ystep = (posValNum.at(posValNum.size() - 2) - posValNum.at(posValNum.size() - 5));
        int Zstep = (posValNum.at(posValNum.size() - 1) - posValNum.at(posValNum.size() - 4));
        //qDebug() << Xstep << " " << Ystep << " " << Zstep;

        if (reversedX) Xstep=-Xstep;
        if (reversedY) Ystep=-Ystep;
        if (reversedZ) Zstep=-Zstep;

        if (Xstep > 0) byte |= 0b00000001;
        if (Ystep > 0) byte |= 0b00000010;
        if (Zstep > 0) byte |= 0b00000100;

        if (Xstep < 0) byte |= 0b00001000;
        if (Ystep < 0) byte |= 0b00010000;
        if (Zstep < 0) byte |= 0b00100000;

        steps.append(byte);
        //qDebug() << byte;
    }
}

void Decoder::writePosFile()
{
    QFile file("output.asc");
    if(file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "File opened";
        QTextStream stream(&file);
        stream << posValues.readAll();
        file.close();
        qDebug() << "Writing finished";
    }
}

void Decoder::writeStepsFile()
{
    QFile file("steps.dat");
    if(file.open(QIODevice::WriteOnly))
    {
        qDebug() << "Binary file opened";
        file.write(steps);
        file.close();
        qDebug() << "Binary writing finished";
    }
}
void Decoder::getBytes()
{
    while ((byteCounter < steps.length()) && ((byteCounter - recCounter) < stepsBufSize))
    {
        quint8 byte = steps.at(byteCounter);
        emit sendSingleByte(byte);
        byteCounter++;
    }
}

void Decoder::incrRecCounter(int val)
{
    recCounter = recCounter + val;
    //qDebug() << recCounter;
    getBytes();
}

void Decoder::resetBuffs()
{
    posVals.clear();
    posValNum.clear();
    posValues.flush();
    steps.clear();
    byteCounter = 0;
    recCounter = 0;
}
