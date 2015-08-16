#ifndef TABBEDMAINWINDOW_H
#define TABBEDMAINWINDOW_H

#include <QMainWindow>
#include "clustertab.h"
#include <QtNetwork/QNetworkReply>

namespace Ui {
class TabbedMainWindow;
}

class TabbedMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit TabbedMainWindow(QWidget *parent = 0);
    ~TabbedMainWindow();

public slots:
    void on_pushButtonFindUsers_clicked();
    void on_comboBoxQueryType_activated(int index);
    void findUsers(QString hostname, QString usernamepassword, int condition1, QString condition2, QString argument, QListWidget * list, QListWidget * list2);
    void addUsersToCluster(QString hostname, QString usernamepassword, QListWidget * list1, QListWidget * list2);
    void removeUsersfromCluster(QString hostname, QString usernamepassword, QListWidget * list1, QListWidget * list2);
    QString getSubstringBetween(QString src, QString start, QString stop);
    void onfinish(QNetworkReply *rep);
    void on_actionBack_to_Cluster_Management_triggered();
    void onError(QNetworkReply::NetworkError reply);

private slots:
    void on_btnAddUsers_clicked();

    void on_btnRemoveUsers_clicked();

private:
    Ui::TabbedMainWindow *ui;
    ClusterTab *clustertab;
};

#endif // TABBEDMAINWINDOW_H
