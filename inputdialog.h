#ifndef INPUTDIALOG_H
#define INPUTDIALOG_H

#include <QDialog>
#include <QString>
#include "clustermanagement.h"

namespace Ui {
class InputDialog;
}

class InputDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InputDialog(QString title, QString desc, QString placeholder);
    ~InputDialog();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::InputDialog *ui;
};

#endif // INPUTDIALOG_H
