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
    bool testConnection();
    void findUsers(QString hostname, QString usernamepassword, int condition1, QString condition2, QString argument, QListWidget * list);
    void onfinish(QNetworkReply *rep);
    void on_actionBack_to_Cluster_Management_triggered();
    void onError(QNetworkReply::NetworkError reply);

private:
    Ui::TabbedMainWindow *ui;
    ClusterTab *clustertab;
};

#endif // TABBEDMAINWINDOW_H
