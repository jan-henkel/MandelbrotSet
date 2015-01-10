#include "mandelbrotmainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MandelbrotMainWindow w;
    w.show();

    return a.exec();
}
