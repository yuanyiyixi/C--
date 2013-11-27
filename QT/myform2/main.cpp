#include "myform.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MyWidget win;
    win.show();

    return app.exec();
}
