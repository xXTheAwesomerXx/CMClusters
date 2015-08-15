#include "progressdialog.h"
#include "ui_progressdialog.h"

ProgressDialog::ProgressDialog(QString title, QString desc) :
    //QDialog(title),
    ui(new Ui::ProgressDialog)
{
    ui->setupUi(this);
    this->setWindowTitle(title);
    ui->labelDesc->setText(desc);
}

ProgressDialog::~ProgressDialog()
{
    delete ui;
}
