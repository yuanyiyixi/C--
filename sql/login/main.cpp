#include <QApplication>
#include "connection.h"
#include "mywidget.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    if (!createConnection())
        return 1;
    MyWidget win;
    win.show();
    return app.exec();
}
