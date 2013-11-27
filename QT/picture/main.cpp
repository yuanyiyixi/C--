#include <QApplication>
#include "mypic.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MyPic win;
    win.show();

    return app.exec();
}
