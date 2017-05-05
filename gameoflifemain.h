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
    ColonyMap *map;

public slots:
    void setMapSize();
    void setInterval();
};

#endif // GAMEOFLIFEMAIN_H
