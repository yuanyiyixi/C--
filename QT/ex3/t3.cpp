#include <QApplication>
#include <QFont>
#include <QPushButton>
#include <QWidget>
#include <QTextCodec>

class MyWidget : public QWidget
{
public:
    MyWidget(QWidget *parent = 0);
};

MyWidget::MyWidget(QWidget *parent) : QWidget(parent)
{
    setFixedSize(200, 120);

    QPushButton *quit = new QPushButton(tr("退出"), this);
    quit->setGeometry(62, 40, 75, 30);
    quit->setFont(QFont("Times", 18, QFont::Bold));

    connect(quit, SIGNAL(clicked()), qApp, SLOT(quit()));
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    MyWidget widget;
    widget.show();
    return app.exec();
}
