#include "inputdialog.h"
#include "ui_inputdialog.h"

InputDialog::InputDialog(QString title, QString desc, QString placeholder) :
    //QDialog(title),
    ui(new Ui::InputDialog)
{
    ui->setupUi(this);
    this->setWindowTitle(title);
    ui->labelDesc->setText(desc);
    ui->lineEditInput->setPlaceholderText(placeholder);
}

InputDialog::~InputDialog()
{

}

void InputDialog::on_buttonBox_accepted()
{

}
