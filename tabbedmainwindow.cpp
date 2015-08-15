#include "tabbedmainwindow.h"
#include "ui_tabbedmainwindow.h"
#include "clustertab.h"
#include <QListWidget>
#include <QList>
#include "variables.h"
#include <clustermanagement.h>
#include <QDebug>
#include <QUrl>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QSslCertificate>
#include <QtNetwork/QSslError>
#include <QtNetwork/QSslKey>
#include <QtNetwork/QSslConfiguration>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QSslSocket>
#include <QEventLoop>
#include <QFile>
#include <QMessageBox>
#include "progressdialog.h"

using namespace Variables;
QVector<QListWidget*> myList;
TabbedMainWindow::TabbedMainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::TabbedMainWindow)
{
    clustertab = new ClusterTab();
    //anothertab = new ClusterTab();
    ui->setupUi(this);
    //ui->tabWidgetClusters->addTab(clustertab, tr("Tab1"));

    for (int i = 0; i < clusterNamesF.count(); i++) {
        ClusterTab * tab = new ClusterTab();
        myList.append(tab->listWidget);
        tab->setHostname(Variables::hostNamesF[i]);
        tab->setClusterName(Variables::clusterNamesF[i]);
        tab->setUsernamePassword(Variables::usernamePasswordsF[i]);
        ui->tabWidgetClusters->addTab(tab, Variables::clusterNamesF[i]);
        //ui->tabWidgetClusters->addTab(new ClusterTab(), QString::fromStdString("Tab") + QString::fromStdString(std::to_string(i+1)));
    }
    QStringList queryTypeList, queryParameter1;
    queryParameter1 << "Equals" << "Contains" << "Begins with" << "Ends with" << "Is Empty" << "Is not Empty";
    queryTypeList << "Firstname" << "Middlename" << "Lastname" << "UserID" << "Is in Cluster";
    ui->comboBoxQueryType->addItems(queryTypeList);
    ui->comboBoxQueryParameter->addItems(queryParameter1);
}

TabbedMainWindow::~TabbedMainWindow()
{
    //delete ui;
}

void TabbedMainWindow::onfinish(QNetworkReply *rep)
{
    QByteArray bts = rep->readAll();
    QString str(bts);
    QMessageBox::information(this,"sal",str,"ok");

}

bool TabbedMainWindow::testConnection() {
    ProgressDialog progbar("CMClusters - Connecting...", "Please wait while a connection to the selected clusters are established!");
    progbar.show();
    QByteArray jsonString = "Some php post data";
    QByteArray postDataSize = QByteArray::number(jsonString.size());
    QUrl req("https://posttestserver.com/post.php");
    QNetworkRequest request(req);

    request.setRawHeader("User-Agent", "Chrome");
    request.setRawHeader("Content-Type", "text/html");
    request.setRawHeader("Content-Length", postDataSize);

    QNetworkAccessManager test;
    QEventLoop loop;
    connect(&test, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
    QNetworkReply * reply = test.post(request, jsonString);
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
    loop.exec();

    QByteArray response = reply->readAll();
    QVariant statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute );
    //progbar.close();//Why does this close, entire application window?
    if ( !statusCode.isValid() ) {
        qDebug() << "Failed...";
        return false;
    }

    int status = statusCode.toInt();

    if ( status != 200 ) {
        QString reason = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
        qDebug() << reason;
    } else {
        qDebug() << "Good reply";
        qDebug() << response;
        return true;
    }
    qDebug() << response;
    return false;
}

void TabbedMainWindow::onError(QNetworkReply::NetworkError rep) {
    qDebug() << "We got an error, quit!";
}

void TabbedMainWindow::on_pushButtonFindUsers_clicked()
{
    for (int i = 0; i < clusterNamesF.count(); i++) {
        qDebug() << usernamePasswordsF[i];
    }
    int selectedType = ui->comboBoxQueryType->currentIndex();
    int selectedParameter = ui->comboBoxQueryParameter->currentIndex();
    for (int i = 0; i < myList.count(); i++) {
        // XXX: use this to update all lists, after getting their list data from getting the endusers
        if (testConnection()) {
            QListWidget * list = myList[i];
            list->addItem("Test to Add Items");
        } else {
            QMessageBox::critical(this, "CMClusters - Error", "Could not connect to cluster!");
        }
    }
}

void TabbedMainWindow::on_comboBoxQueryType_activated(int index)
{
    QStringList queryParameter12, queryParameter22;
    queryParameter12 << "Equals" << "Contains" << "Begins with" << "Ends with" << "Is Empty" << "Is not Empty";
    queryParameter22 << "True" << "False";
    if (index == 4) {
        ui->comboBoxQueryParameter->clear();
        ui->comboBoxQueryParameter->addItems(queryParameter22);
    } else {
        ui->comboBoxQueryParameter->clear();
        ui->comboBoxQueryParameter->addItems(queryParameter12);
    }
}

void TabbedMainWindow::on_actionBack_to_Cluster_Management_triggered()
{
    this->hide();
    this->parentWidget()->show();
    Variables::clusterNamesF.clear();
    Variables::hostNamesF.clear();
    Variables::usernamePasswordsF.clear();
}

