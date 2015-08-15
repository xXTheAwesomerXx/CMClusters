#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H

#include <QDialog>

namespace Ui {
class ProgressDialog;
}

class ProgressDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProgressDialog(QString title, QString desc);
    ~ProgressDialog();

private:
    Ui::ProgressDialog *ui;
};

#endif // PROGRESSDIALOG_H
