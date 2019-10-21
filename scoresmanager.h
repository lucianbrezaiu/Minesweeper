#ifndef SCORESMANAGER_H
#define SCORESMANAGER_H

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDebug>

class ScoresManager
{
public:
    ScoresManager();

    void read();

    void write();

    QString toString();

private:
    int easy;
    int medium;
    int hard;
    QFile file;
};

#endif // SCORESMANAGER_H
