#include <QApplication>
#include "mytime.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MyTime win;
    win.show();

    return app.exec();
}
