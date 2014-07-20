#include "mainwindow.h"
#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QStyle>

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	QTranslator qtTranslator;
	qtTranslator.load(":/translations/qt_" + QLocale::system().name());
	app.installTranslator(&qtTranslator);

	QTranslator qdir2modTranslator;
	qdir2modTranslator.load(":/translations/qdir2mod_" + QLocale::system().name());
	app.installTranslator(&qdir2modTranslator);

	MainWindow *window = new MainWindow();
	window->show();
	
	return app.exec();
}
