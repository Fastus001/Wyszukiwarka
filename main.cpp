#include "wyszukiwanie.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    wyszukiwanie w;
    w.show();

    return a.exec();
}
