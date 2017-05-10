#include "qtopengl.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	/*
	QApplication a(argc, argv);
	qtopengl w;
	w.show();
	w.init();
	w.start();
	return a.exec();
	*/


	QApplication a(argc, argv);
	qtopengl w;
	w.show();
	w.glstart();
	return a.exec();
}
