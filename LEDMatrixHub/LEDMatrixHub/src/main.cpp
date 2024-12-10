#include <QtWidgets/QApplication>
#include "LEDMatrixHub.hpp"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LEDMatrixHub w;
    w.show();
    return a.exec();
}
