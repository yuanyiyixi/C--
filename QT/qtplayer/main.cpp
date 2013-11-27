#include <QApplication>
#include "myplayer.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MyPlayer win;
    win.setAutoFillBackground(true);
    QPalette palette;
           palette.setBrush(QPalette::Background, QBrush(QPixmap("./files/28.jpg")));
           win.setPalette(palette);
    win.show();
    return app.exec();
}

