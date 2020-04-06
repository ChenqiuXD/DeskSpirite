#include "deskspirite.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DeskSpirite w;
    w.show();

    return a.exec();
}
