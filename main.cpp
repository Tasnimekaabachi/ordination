#include "gcandidat.h"
#include "connection.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GCANDIDAT w;
    w.show();
    return a.exec();
}
