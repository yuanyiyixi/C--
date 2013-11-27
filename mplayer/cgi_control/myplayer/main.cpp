#include <QApplication>
#include "myform.h"

int main(int argc, char *argv[]) 
{
	QApplication app(argc, argv);

	MyForm window;
	window.show();

	return app.exec();
}
