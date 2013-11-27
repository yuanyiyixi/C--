#include <QtGui/QApplication>
#include "widget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;//等价于w(0)

    //NavigationKeyFilter filter;
    Widget filter;
    a.installEventFilter(&filter);

    w.show();//w.hide();
    //w.setVisible(true);//invisible water
    return a.exec();
}
