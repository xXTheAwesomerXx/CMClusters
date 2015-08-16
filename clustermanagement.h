#ifndef CLUSTERMANAGEMENT_H
#define CLUSTERMANAGEMENT_H

#include <QMainWindow>
#include <QString>
#include <QListWidgetItem>
#include "tabbedmainwindow.h"
#include "clustertab.h"

namespace Ui {
class ClusterManagement;
}

class ClusterManagement : public QMainWindow
{
    Q_OBJECT

public:
    explicit ClusterManagement(QWidget *parent = 0);
    ~ClusterManagement();

private slots:
    void on_BtnAddClusters_clicked();
    void on_actionExit_triggered();
    void addListItem(QString);
    bool addConnection(QString, QString);
    void on_actionClusters_Details_triggered();
    void on_actionAs_Tab_triggered();
    void onfinish(QNetworkReply *rep);
    void onError(QNetworkReply::NetworkError reply);
    void on_actionTest_Connections_triggered();

    void on_btnNext_clicked();

    void on_btnTestConnections_clicked();

private:
    Ui::ClusterManagement *ui;
    TabbedMainWindow *tabbedwindow;
};

#endif // CLUSTERMANAGEMENT_H
