#ifndef VARIABLES_H
#define VARIABLES_H
#include <string>
#include <QListWidget>
#include <QVector>

namespace Variables
{
    extern QVector<QListWidget*> list; //QList of Pointers
    extern QVector<QString> clusterNames;
    extern QVector<QString> hostNames;
    extern QVector<QString> usernamePasswords;
    extern QVector<QString> clusterNamesF;
    extern QVector<QString> hostNamesF;
    extern QVector<QString> usernamePasswordsF;
}
#endif // VARIABLES_H
