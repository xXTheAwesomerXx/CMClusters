#ifndef CLUSTERTAB_H
#define CLUSTERTAB_H

#include <QTabWidget>
#include <QListWidget>

namespace Ui {
class ClusterTab;
}

class ClusterTab : public QWidget
{
    Q_OBJECT

public:
    explicit ClusterTab(QWidget *parent = 0);
    ~ClusterTab();
    QListWidget *listWidget;
    void addListItem(QString);
    QString getClusterName();
    QString getHostname();
    QString getUsernamePassword();
    void setClusterName(QString);
    void setHostname(QString);
    void setUsernamePassword(QString);
private:
    Ui::ClusterTab *ui;
    QString clusterName;
    QString hostName;
    QString usernamePassword;

};

#endif // CLUSTERTAB_H
