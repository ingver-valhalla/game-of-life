#include "gameoflifemain.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    GameOfLifeMain w;
//    w.show();

    ColonyMap w(NULL, 30, 30);
    w.show();


    return a.exec();
}
