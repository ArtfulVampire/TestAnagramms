#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//	MainWindow::testFileHash("/home/michael/Projects/build-TestAnagramms/out.txt");
//	exit(0);

    MainWindow w;
    w.show();

	return a.exec();
}
