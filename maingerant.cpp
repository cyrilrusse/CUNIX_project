#include "windowgerant.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    WindowGerant w;
    w.show();
    return a.exec();
}
