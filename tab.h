#include <QWidget>
#include <QListWidget>
class Tab : QWidget
{
    Q_OBJECT

    public:
    QListWidget* listWidget();
    void addListWidget(QListWidget* listWidget);

    private:
    QListWidget* ListWidget;
};
