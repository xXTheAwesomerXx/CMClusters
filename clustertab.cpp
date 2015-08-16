#include "clustertab.h"
#include "ui_clustertab.h"
#include "variables.h"
#include <QDebug>
#include <QListWidget>
#include <QListWidgetItem>

ClusterTab::ClusterTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClusterTab)
{
    ui->setupUi(this);
    listWidget = new QListWidget;
    listWidget2 = new QListWidget;
    ui->scrollArea->setWidget(listWidget);
    ui->scrollArea_2->setWidget(listWidget2);
}

void ClusterTab::addListItem(QString newItem)
{
    listWidget->addItem(newItem);
}

QString ClusterTab::getClusterName() {
    return clusterName;
}

QString ClusterTab::getHostname() {
    return hostName;
}

QString ClusterTab::getUsernamePassword() {
    return usernamePassword;
}

void ClusterTab::setClusterName(QString name) {
    clusterName = name;
}

void ClusterTab::setHostname(QString name) {
    hostName = name;
}

void ClusterTab::setUsernamePassword(QString usernamepassword) {
    usernamePassword = usernamepassword;
}

ClusterTab::~ClusterTab()
{
    //delete ui;
}
