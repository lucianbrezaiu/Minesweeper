#include "scoresmanager.h"


ScoresManager::ScoresManager()
{

}

void ScoresManager::read()
{
    QFile file("E:\workspace - qt\c++ tema_3\Resources\scores.txt");
    if (file.open(QIODevice::ReadOnly))
    {
        int ok = 0;
        QTextStream in(&file);
        while (!in.atEnd())
        {

           ok++;
           QString line = in.readLine();
           line.toInt();
           switch(ok)
           {
           case 1:
               easy = line.toInt();
               break;
           case 2:
               medium = line.toInt();
               break;
           case 3:
               hard = line.toInt();
               break;

           }
        }
        file.close();
    }
}

void ScoresManager::write()
{


}

QString ScoresManager::toString()
{
    return "Easy: " + QString::number(easy) + " Medium: " + QString::number(medium) + " Hard: " + QString::number(hard);
}
