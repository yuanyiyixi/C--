#include <QApplication>
#include "mycp.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MyCp win;
    win.show();

    return app.exec();
}
