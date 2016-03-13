#include "gameoflifemain.h"
#include "ui_gameoflifemain.h"

GameOfLifeMain::GameOfLifeMain(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GameOfLifeMain),
    map(new ColonyMap(this, 100, 100 ))

{
    ui->setupUi(this);
    ui->mapLayout->addWidget(map);
}

GameOfLifeMain::~GameOfLifeMain()
{
    delete ui;
}
