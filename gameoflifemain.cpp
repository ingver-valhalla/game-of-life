#include "gameoflifemain.h"
#include "ui_gameoflifemain.h"

GameOfLifeMain::GameOfLifeMain(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GameOfLifeMain)
{
    ui->setupUi(this);
}

GameOfLifeMain::~GameOfLifeMain()
{
    delete ui;
}
