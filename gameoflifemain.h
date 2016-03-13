#ifndef GAMEOFLIFEMAIN_H
#define GAMEOFLIFEMAIN_H

#include <QMainWindow>
#include "colonymap.h"

namespace Ui {
class GameOfLifeMain;
}

class GameOfLifeMain : public QMainWindow
{
    Q_OBJECT

public:
    explicit GameOfLifeMain(QWidget *parent = 0);
    ~GameOfLifeMain();

private:
    Ui::GameOfLifeMain *ui;
    ColonyMap * map;
};

#endif // GAMEOFLIFEMAIN_H
