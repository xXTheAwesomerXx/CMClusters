#include "clustermanagement.h"
#include "ui_clustermanagement.h"
#include "variables.h"
#include "tabbedmainwindow.h"
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
#include <string>
#include <vector>
#include "variables.h"

using namespace Variables;
QString base64_encode(QString string);
QString base64_decode(QString string);
ClusterManagement::ClusterManagement(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ClusterManagement)
{
    ui->setupUi(this);
}

ClusterManagement::~ClusterManagement()
{
    delete ui;
}

void ClusterManagement::addListItem(QString newItem)
{
    ui->listClusters->addItem(newItem);
}

bool ClusterManagement::addConnection(QString hostname, QString usernamepassword) {
    ProgressDialog progbar("CMClusters - Connecting...", "Please wait while a connection to the selected clusters are established!");
    progbar.show();
    QByteArray jsonString = "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:ns=\"http://www.cisco.com/AXL/API/8.5\">";
    jsonString += "<soapenv:Body><ns:executeSQLQuery><sql>SELECT enduser.userid,enduser.firstname,enduser.middlename,enduser.lastname,enduser.islocaluser from enduser where enduser.firstname LIKE 'A%'</sql></ns:executeSQLQuery></SOAP-ENV:Envelope>";
    QByteArray postDataSize = QByteArray::number(jsonString.size());
    QUrl req("https://" + hostname.toLocal8Bit() + ":8443/axl/");
    QNetworkRequest request(req);

    request.setRawHeader("SOAPAction", "\"CUCM:DB ver=8.5 executeSQLQuery\"");
    request.setRawHeader("Authorization", "Basic " + usernamepassword.toLocal8Bit());
    request.setRawHeader("Content-Type", "text/xml");
    request.setRawHeader("Content-Length", postDataSize);

    QNetworkAccessManager test;
    QEventLoop loop;
    connect(&test, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
    QNetworkReply * reply = test.post(request, jsonString);
    reply->ignoreSslErrors(); // Ignore only unsigned later on
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
    loop.exec();

    QByteArray response = reply->readAll();
    QVariant statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute );
    //progbar.close();//Why does this close, entire application window?
    if ( !statusCode.isValid() ) {
        qDebug() << "Failed...";
        qDebug() << statusCode.data();
        return false;
    }

    int status = statusCode.toInt();

    if ( status != 200 ) {
        QString reason = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
        qDebug() << reason;
        return false;
    } else {
        qDebug() << "Good reply";
        qDebug() << response;
        return true;
    }
    qDebug() << response;
    return false;
}

void ClusterManagement::onfinish(QNetworkReply *rep)
{
    QByteArray bts = rep->readAll();
    QString str(bts);
    QMessageBox::information(this,"sal",str,"ok");

}

void ClusterManagement::onError(QNetworkReply::NetworkError rep) {
    qDebug() << "We got an error, quit!";
}

QString base64_encode(QString string){
    QByteArray ba;
    ba.append(string);
    return ba.toBase64();
}

QString base64_decode(QString string){
    QByteArray ba;
    ba.append(string);
    return QByteArray::fromBase64(ba);
}

void ClusterManagement::on_BtnAddClusters_clicked() {
    if ((!ui->lineEditHostname->text().isEmpty()) && (!ui->lineEditUsername->text().isEmpty()) && (!ui->lineEditPassword->text().isEmpty())) {
        if (!ui->lineEditClusterName->text().isEmpty()) {
            QList<QListWidgetItem *> items = ui->listClusters->findItems(ui->lineEditClusterName->text(), Qt::MatchExactly);
            if (items.size() > 0) {
                qDebug() << "Something was found with that name...";
                QMessageBox::critical(this, "CMClusters - Error", "A cluster was already found with that name! *You can leave the cluster name field blank and the added cluster will have a name of the clusters Hostname/IP by default.");
            } else {
                if (ui->checkBoxConnect->isChecked()) {
                    if (addConnection(ui->lineEditHostname->text(), base64_encode(ui->lineEditUsername->text() + QString::fromStdString(":") + ui->lineEditPassword->text()))) {
                        qDebug() << "Item not found, adding!";
                        QListWidgetItem* cluster = new QListWidgetItem(ui->lineEditClusterName->text(), ui->listClusters);
                        cluster->setFlags(cluster->flags() | Qt::ItemIsUserCheckable = false); //Let's make it so
                        cluster->setFlags(cluster->flags() | Qt::ItemIsSelectable);
                        cluster->setCheckState(Qt::Checked);
                        cluster->setFlags(cluster->flags() | Qt::ItemIsEnabled);
                        Variables::clusterNames.append(ui->lineEditClusterName->text());
                        Variables::hostNames.append(ui->lineEditHostname->text());
                        Variables::usernamePasswords.append(base64_encode(ui->lineEditUsername->text() + QString::fromStdString(":") + ui->lineEditPassword->text()));
                    } else {
                        // Maybe popup here? because we weren't able to connect
                        qDebug() << "Item not found, adding!";
                        QListWidgetItem* cluster = new QListWidgetItem(ui->lineEditClusterName->text(), ui->listClusters);
                        cluster->setFlags(cluster->flags() | Qt::ItemIsUserCheckable = false); //Let's make it so
                        cluster->setFlags(cluster->flags() | Qt::ItemIsSelectable);
                        cluster->setSelected(true);
                        cluster->setCheckState(Qt::Unchecked); //XXX: Figure out how to explicitly
                        cluster->setFlags(cluster->flags() | Qt::ItemIsEnabled);
                        Variables::clusterNames.append(ui->lineEditClusterName->text());
                        Variables::hostNames.append(ui->lineEditHostname->text());
                        Variables::usernamePasswords.append(base64_encode(ui->lineEditUsername->text() + QString::fromStdString(":") + ui->lineEditPassword->text()));
                    }
                } else {
                    // Just add the item
                    qDebug() << "Item not found, adding!";
                    QListWidgetItem* cluster = new QListWidgetItem(ui->lineEditClusterName->text(), ui->listClusters);
                    cluster->setFlags(cluster->flags() | Qt::ItemIsUserCheckable = false); //Let's make it so
                    cluster->setFlags(cluster->flags() | Qt::ItemIsSelectable);
                    cluster->setSelected(true);
                    cluster->setCheckState(Qt::Unchecked); //XXX: Figure out how to explicitly
                    cluster->setFlags(cluster->flags() | Qt::ItemIsEnabled);
                    Variables::clusterNames.append(ui->lineEditClusterName->text());
                    Variables::hostNames.append(ui->lineEditHostname->text());
                    Variables::usernamePasswords.append(base64_encode(ui->lineEditUsername->text() + QString::fromStdString(":") + ui->lineEditPassword->text()));
                }
            }
        } else {
            QList<QListWidgetItem *> items = ui->listClusters->findItems(ui->lineEditHostname->text(), Qt::MatchExactly);
            if (items.size() > 0) {
                qDebug() << "Something was found with that name...";
                QMessageBox::critical(this, "CMClusters - Error", "A cluster was already found with that name! *You can leave the cluster name field blank and the added cluster will have a name of the clusters Hostname/IP by default.");
            } else {
                if (ui->checkBoxConnect->isChecked()) {
                    if (addConnection(ui->lineEditHostname->text(), base64_encode(ui->lineEditUsername->text() + QString::fromStdString(":") + ui->lineEditPassword->text()))) {
                        qDebug() << "Item not found, adding!";
                        QListWidgetItem* cluster = new QListWidgetItem(ui->lineEditHostname->text(), ui->listClusters);
                        cluster->setFlags(cluster->flags() | Qt::ItemIsUserCheckable = false); //Let's make it so
                        cluster->setFlags(cluster->flags() | Qt::ItemIsSelectable);
                        cluster->setCheckState(Qt::Checked);
                        cluster->setFlags(cluster->flags() | Qt::ItemIsEnabled);
                        Variables::clusterNames.append(ui->lineEditHostname->text());
                        Variables::hostNames.append(ui->lineEditHostname->text());
                        Variables::usernamePasswords.append(base64_encode(ui->lineEditUsername->text() + QString::fromStdString(":") + ui->lineEditPassword->text()));
                    } else {
                        // Maybe popup here? because we weren't able to connect
                        qDebug() << "Item not found, adding!";
                        QListWidgetItem* cluster = new QListWidgetItem(ui->lineEditHostname->text(), ui->listClusters);
                        cluster->setFlags(cluster->flags() | Qt::ItemIsUserCheckable = false); //Let's make it so
                        cluster->setFlags(cluster->flags() | Qt::ItemIsSelectable);
                        cluster->setSelected(true);
                        cluster->setCheckState(Qt::Unchecked); //XXX: Figure out how to explicitly
                        cluster->setFlags(cluster->flags() | Qt::ItemIsEnabled);
                        Variables::clusterNames.append(ui->lineEditHostname->text());
                        Variables::hostNames.append(ui->lineEditHostname->text());
                        Variables::usernamePasswords.append(base64_encode(ui->lineEditUsername->text() + QString::fromStdString(":") + ui->lineEditPassword->text()));
                    }
                } else {
                    // Just add the item
                    qDebug() << "Item not found, adding!";
                    QListWidgetItem* cluster = new QListWidgetItem(ui->lineEditHostname->text(), ui->listClusters);
                    cluster->setFlags(cluster->flags() | Qt::ItemIsUserCheckable = false); //Let's make it so
                    cluster->setFlags(cluster->flags() | Qt::ItemIsSelectable);
                    cluster->setSelected(true);
                    cluster->setCheckState(Qt::Unchecked); //XXX: Figure out how to explicitly
                    cluster->setFlags(cluster->flags() | Qt::ItemIsEnabled);
                    Variables::clusterNames.append(ui->lineEditHostname->text());
                    Variables::hostNames.append(ui->lineEditHostname->text());
                    Variables::usernamePasswords.append(base64_encode(ui->lineEditUsername->text() + QString::fromStdString(":") + ui->lineEditPassword->text()));
                }
            }
        }
    } else {
        QMessageBox::critical(this, "CMClusters - Error", "One of fields Hostname, Username, or Password has no value!");
    }

}

void ClusterManagement::on_actionExit_triggered()
{
    ClusterManagement::close();
}

void ClusterManagement::on_actionClusters_Details_triggered()
{
    // TODO: Add support for viewing cluster details
}

void ClusterManagement::on_actionAs_Tab_triggered()
{
    int numOfConns = 0;
    for (int i = 0; i < ui->listClusters->count(); i++) {
        if (ui->listClusters->item(i)->checkState() == Qt::Checked) {
            Variables::clusterNamesF.append(Variables::clusterNames[i]);
            Variables::hostNamesF.append(Variables::hostNames[i]);
            Variables::usernamePasswordsF.append(Variables::usernamePasswords[i]);
            numOfConns++;
        }
    }
    if (numOfConns > 0) {
        tabbedwindow = new TabbedMainWindow(this);
        tabbedwindow->show();
        this->hide();//Correct 'hiding' so that we won't have to RELOAD contact points
    } else {
        QMessageBox::critical(this, "CMClusters - Error", "No clusters were selected! Please select at least one cluster and try again. \n\n Before being able to select a cluster, you must first test the connection!");
    }
}

void ClusterManagement::on_actionTest_Connections_triggered()
{
    for (int i = 0; i < ui->listClusters->count(); i++) {
        if (ui->listClusters->item(i)->isSelected())
            if (addConnection(Variables::hostNames[i], Variables::usernamePasswords[i]))
                ui->listClusters->item(i)->setCheckState(Qt::Checked);
    }
}
