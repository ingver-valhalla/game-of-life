#include "gameoflifemain.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GameOfLifeMain w;
    w.show();

    return a.exec();
}
