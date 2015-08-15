#include "clustermanagement.h"
#include "variables.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ClusterManagement w;
    w.show();
    qDebug() << "Support SSL:  " << QSslSocket::supportsSsl()
            << "\nLib Version Number: " << QSslSocket::sslLibraryVersionNumber()
            << "\nLib Version String: " << QSslSocket::sslLibraryVersionString()
            << "\nLib Build Version Number: " << QSslSocket::sslLibraryBuildVersionNumber()
            << "\nLib Build Version String: " << QSslSocket::sslLibraryBuildVersionString();
    return a.exec();
}
