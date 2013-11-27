#include <QApplication>
#include <QPushButton>
#include <QWidget>

class MyWidget : public QWidget
{
    //有其他成员初始化时，一般把创建的父对象放最后，表示没有父对象
   // MyWidget(int mya, char myb, QWidget *parent = 0);
    MyWidget(QWidget *parent = 0);
private:
    //int a;
    //char
};

MyWidget::MyWidget(QWidget *parent)
:QWidget(parent)
{
    setGeometry(0, 0, 800, 600);
    QPushButton *hello = new QPushButton("Hello world", this);
    hello->resize(100, 50);
    hello->setGeometry((width()-hello->width())/2,
                             (height()-hello->height())/2,
                             hello->width(),
                             hello->height());
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MyWidget win;

    win.show();
   // hello.show();

    return app.exec();
}
