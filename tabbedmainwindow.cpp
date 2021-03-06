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
#include <QDomDocument>
#include <QColor>

using namespace Variables;
QVector<QListWidget*> myList;
QVector<QListWidget*> myList2;
TabbedMainWindow::TabbedMainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::TabbedMainWindow)
{
    clustertab = new ClusterTab();
    //anothertab = new ClusterTab();
    ui->setupUi(this);
    //ui->tabWidgetClusters->addTab(clustertab, tr("Tab1"));

    for (int i = 0; i < clusterNamesF.count(); i++) {
        ClusterTab * tab = new ClusterTab();
        myList.append(tab->listWidget);
        myList2.append(tab->listWidget2);
        tab->listWidget->setStyleSheet("QListView::item:selected:active { background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(58, 58, 58, 255), stop:1 rgba(90, 90, 90, 255)) } QListView::item:hover { background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #FAFBFE, stop: 1 #DCDEF1); }");
        tab->listWidget2->setStyleSheet("QListView::item:selected:active { background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(58, 58, 58, 255), stop:1 rgba(90, 90, 90, 255)) } QListView::item:hover { background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #FAFBFE, stop: 1 #DCDEF1); }");
        tab->listWidget->setSelectionMode(QAbstractItemView::MultiSelection);
        tab->listWidget2->setSelectionMode(QAbstractItemView::MultiSelection);
        tab->setHostname(Variables::hostNamesF[i]);
        tab->setClusterName(Variables::clusterNamesF[i]);
        tab->setUsernamePassword(Variables::usernamePasswordsF[i]);
        ui->tabWidgetClusters->addTab(tab, Variables::clusterNamesF[i]);
        //ui->tabWidgetClusters->addTab(new ClusterTab(), QString::fromStdString("Tab") + QString::fromStdString(std::to_string(i+1)));
    }
    QStringList queryTypeList, queryParameter1;
    queryParameter1 << "Equals" << "Contains" << "Begins with" << "Ends with" << "Is Empty" << "Is not Empty";
    queryTypeList << "Firstname" << "Middlename" << "Lastname" << "UserID" << "Is in Cluster" << "Telephone Number";
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

void TabbedMainWindow::onError(QNetworkReply::NetworkError rep) {
    qDebug() << "We got an error, quit!";
}

void TabbedMainWindow::on_pushButtonFindUsers_clicked()
{
    for (int i = 0; i < clusterNamesF.count(); i++) {
        qDebug() << usernamePasswordsF[i];
    }
    int selectedType = ui->comboBoxQueryType->currentIndex();
    QString selectedParameter = ui->comboBoxQueryParameter->currentText();
    QString argumentData = ui->lineEditQueryData->text();
    for (int i = 0; i < myList.count(); i++) {
        // XXX: use this to update all lists, after getting their list data from getting the endusers
        QListWidget * list = myList[i];
        QListWidget * list2 = myList2[i];
        list->clear();
        list2->clear();
        findUsers(Variables::hostNamesF[i], Variables::usernamePasswordsF[i], selectedType, selectedParameter, argumentData, list, list2);
    }
}

void TabbedMainWindow::findUsers(QString hostname, QString usernamepassword, int condition1, QString condition2, QString argument, QListWidget * list, QListWidget * list2) {
    //First maybe we clear the dang list?!
    QString condition1String, condition2String;

    if (condition1 == 0) {
        condition1String = "enduser.firstname";
    } else if (condition1 == 1) {
        condition1String = "enduser.middlename";
    } else if (condition1 == 2) {
        condition1String = "enduser.lastname";
    } else if (condition1 == 3) {
        condition1String = "enduser.userid";
    } else if (condition1 == 4) {
        condition1String = "enduser.islocaluser";
    } else if (condition1 == 5) {
        condition1String = "enduser.telephonenumber";
    }

    if (condition2 == "Equals") {
        condition2String = "= '" + argument.toLower() + "'";
    } else if (condition2 == "Contains") {
        condition2String = "LIKE '%" + argument.toLower() + "%'";
    } else if (condition2 == "Begins with") {
        condition2String = "LIKE '" + argument.toLower() + "%'";
    } else if (condition2 == "Ends with") {
        condition2String = "LIKE '%" + argument.toLower() + "'";
    } else if (condition2 == "Is Empty") {
        condition2String = "IS NULL";
    } else if (condition2 == "Is not Empty") {
        condition2String = "IS NOT NULL";
    } else if (condition2 == "True") {
        condition2String = "= 't'";
    } else if (condition2 == "False") {
        condition2String = "= 'f'";
    }

    ProgressDialog progbar("CMClusters - Connecting...", "Please wait while a connection to the selected clusters are established!");
    progbar.show();
    QByteArray jsonString = "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:ns=\"http://www.cisco.com/AXL/API/8.5\">";
    if (condition1 == 4) {
        jsonString += "<soapenv:Body><ns:executeSQLQuery><sql>SELECT enduser.userid,enduser.firstname,enduser.middlename,enduser.lastname,enduser.islocaluser,enduser.telephonenumber from enduser where "
                + condition1String + condition2String + "</sql></ns:executeSQLQuery></SOAP-ENV:Envelope>";
    } else {
    jsonString += "<soapenv:Body><ns:executeSQLQuery><sql>SELECT enduser.userid,enduser.firstname,enduser.middlename,enduser.lastname,enduser.islocaluser,enduser.telephonenumber from enduser where lower("
            + condition1String + ") " + condition2String + "</sql></ns:executeSQLQuery></SOAP-ENV:Envelope>";
    }
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

    if ( !statusCode.isValid() ) {
        qDebug() << "Failed...";
        qDebug() << statusCode.data();
    }

    int status = statusCode.toInt();

    if ( status != 200 ) {
        QString reason = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
        qDebug() << reason;
    } else {
        qDebug() << "Good reply";
        //qDebug() << response;
        QDomDocument doc;
        doc.setContent(response);
            QDomNodeList rates = doc.elementsByTagName("row");
            for (int i = 0; i < rates.size(); i++) {
                QString finalString;
                QDomNode n = rates.item(i);
                QDomElement firstname = n.firstChildElement("firstname");
                if (firstname.text() != "")
                    finalString = firstname.text() + " ";
                QDomElement middlename = n.firstChildElement("middlename");
                if (middlename.text() != "")
                    finalString += middlename.text() + " ";
                QDomElement lastname = n.firstChildElement("lastname");
                if (lastname.text() != " ")
                    finalString += lastname.text();
                QDomElement userid = n.firstChildElement("userid");
                finalString += " (" + userid.text() + ") ";
                QDomElement islocaluser = n.firstChildElement("islocaluser");
                QDomElement telephonenumber = n.firstChildElement("telephonenumber");
                    if (telephonenumber.text() != "") {
                        finalString += " [" + telephonenumber.text() + "]";
                    } else {
                        finalString += " [No Telephone Number]";
                    }
                QListWidgetItem* user = new QListWidgetItem(finalString);
                if (islocaluser.text() == "t") {
                    user->setBackground(QColor(0, 170, 255));
                    list2->addItem(user);
                } else {
                    user->setBackground(QColor(170, 0, 0));
                    list->addItem(user);
                }
            }
    }
}

QString TabbedMainWindow::getSubstringBetween(QString src, QString start, QString stop) {
    std::string sourceStd = src.toStdString();
    std::string startStd = start.toStdString();
    std::string stopStd = stop.toStdString();
    unsigned first = sourceStd.find(startStd);
    unsigned last = sourceStd.find(stopStd);
    return QString::fromStdString(sourceStd.substr(first, last - first).erase(0, start.length()));
}

void TabbedMainWindow::addUsersToCluster(QString hostname, QString usernamepassword, QListWidget *list1, QListWidget *list2) {
    ProgressDialog progbar("CMClusters - Connecting...", "Please wait while a connection to the selected clusters are established!");
    progbar.show();

    for (int i = list1->count(); i --> 0;) {
        if (list1->item(i)->isSelected()) {
        QString userid = getSubstringBetween(list1->item(i)->text(), QString("("), QString(")"));
        qDebug() << userid;
        QListWidgetItem* userToRemove = list1->item(i);
        QListWidgetItem* userToAdd = new QListWidgetItem(list1->item(i)->text());
        QByteArray jsonString = "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:ns=\"http://www.cisco.com/AXL/API/8.5\">";
        jsonString += "<soapenv:Body><ns:executeSQLUpdate><sql>UPDATE enduser SET enduser.islocaluser = 't' WHERE enduser.userid = '" + userid.toLocal8Bit() + "'</sql></ns:executeSQLUpdate></SOAP-ENV:Envelope>";
        QByteArray postDataSize = QByteArray::number(jsonString.size());
        QUrl req("https://" + hostname.toLocal8Bit() + ":8443/axl/");
        QNetworkRequest request(req);

        request.setRawHeader("SOAPAction", "\"CUCM:DB ver=8.5 executeSQLUpdate\"");
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

        if ( !statusCode.isValid() ) {
            qDebug() << "Failed...";
            qDebug() << statusCode.data();
        }

        int status = statusCode.toInt();

        if ( status != 200 ) {
            QString reason = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
            qDebug() << reason;
        } else {
            qDebug() << "Good reply";
            qDebug() << response;
            if (response.contains("<rowsUpdated>1</rowsUpdated>")) {
                qDebug() << "Something went correct!";
                delete userToRemove;
                list2->addItem(userToAdd);
                userToAdd->setBackground(QColor(0, 170, 255));//Correct the color scheme
            } else {
                qDebug() << "We fucked up!";
            }
        }
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

void TabbedMainWindow::removeUsersfromCluster(QString hostname, QString usernamepassword, QListWidget *list1, QListWidget *list2) {
    ProgressDialog progbar("CMClusters - Connecting...", "Please wait while a connection to the selected clusters are established!");
    progbar.show();

    for (int i = list1->count(); i --> 0;) {
        if (list1->item(i)->isSelected()) {
        QString userid = getSubstringBetween(list1->item(i)->text(), QString("("), QString(")"));
        qDebug() << userid;
        QListWidgetItem* userToRemove = list1->item(i);
        QListWidgetItem* userToAdd = new QListWidgetItem(list1->item(i)->text());
        QByteArray jsonString = "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:ns=\"http://www.cisco.com/AXL/API/8.5\">";
        jsonString += "<soapenv:Body><ns:executeSQLUpdate><sql>UPDATE enduser SET enduser.islocaluser = 'f' WHERE enduser.userid = '" + userid.toLocal8Bit() + "'</sql></ns:executeSQLUpdate></SOAP-ENV:Envelope>";
        QByteArray postDataSize = QByteArray::number(jsonString.size());
        QUrl req("https://" + hostname.toLocal8Bit() + ":8443/axl/");
        QNetworkRequest request(req);

        request.setRawHeader("SOAPAction", "\"CUCM:DB ver=8.5 executeSQLUpdate\"");
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

        if ( !statusCode.isValid() ) {
            qDebug() << "Failed...";
            qDebug() << statusCode.data();
        }

        int status = statusCode.toInt();

        if ( status != 200 ) {
            QString reason = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
            qDebug() << reason;
        } else {
            qDebug() << "Good reply";
            qDebug() << response;
            if (response.contains("<rowsUpdated>1</rowsUpdated>")) {
                qDebug() << "Something went correct!";
                delete userToRemove;
                list2->addItem(userToAdd);
                userToAdd->setBackground(QColor(170, 0, 0));//Correct the color scheme
            } else {
                qDebug() << "We fucked up!";
            }
        }
    }
    }
}

void TabbedMainWindow::on_btnAddUsers_clicked()
{
    // make sure this doesn't use the list count as the list, but rather the current index
    //of the tab
    for (int i = 0; i < myList.count(); i++) {
        // XXX: use this to update all lists, after getting their list data from getting the endusers
        QListWidget * list = myList[i];
        QListWidget * list2 = myList2[i];
        addUsersToCluster(Variables::hostNamesF[i], Variables::usernamePasswordsF[i], list, list2);
    }
}

void TabbedMainWindow::on_btnRemoveUsers_clicked()
{
    // make sure this doesn't use the list count as the list, but rather the current index
    //of the tab
    for (int i = 0; i < myList.count(); i++) {
        // XXX: use this to update all lists, after getting their list data from getting the endusers
        QListWidget * list = myList2[i];
        QListWidget * list2 = myList[i];
        removeUsersfromCluster(Variables::hostNamesF[i], Variables::usernamePasswordsF[i], list, list2);
    }
}
