#include "gameoflifemain.h"
#include "ui_gameoflifemain.h"

GameOfLifeMain::GameOfLifeMain(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GameOfLifeMain),
    map(new ColonyMap(this))

{
    ui->setupUi(this);

    // place colony widget on window
    QPalette Pal(palette());
    Pal.setColor(QPalette::Background, Qt::white);
    map->setAutoFillBackground(true);
    map->setPalette(Pal);
    ui->mapLayout->addWidget(map);

    // setting connections

    // connect control buttons to main slots of map
    connect(ui->startButton, SIGNAL(clicked(bool)),
            map, SLOT(gameStart()));
    connect(ui->stopButton, SIGNAL(clicked(bool)),
            map, SLOT(gameStop()));
    connect(ui->stepButton, SIGNAL(clicked(bool)),
            map, SLOT(nextGen()));
    connect(ui->resetButton, SIGNAL(clicked(bool)),
            map, SLOT(gameReset()));
    connect(ui->cleanColonyButton, SIGNAL(clicked(bool)),
            map, SLOT(cleanMap()));

    // saving and loading
    connect(ui->saveButton, SIGNAL(clicked(bool)),
            map, SLOT(saveMap()));
    connect(ui->loadButton, SIGNAL(clicked(bool)),
            map, SLOT(loadMap()));
    // cell color control
    connect(ui->cellColorButton, SIGNAL(clicked(bool)),
            map, SLOT(chooseCellColor()));

    // let size slider set map size
    connect(ui->colonySizeSlider, SIGNAL(valueChanged(int)),
            map, SLOT(setMapSize(int)));
    // while slider pressed, pause the game
    connect(ui->colonySizeSlider, SIGNAL(sliderPressed()),
            map, SLOT(gamePause()));
    // resume, when slider released
    connect(ui->colonySizeSlider, SIGNAL(sliderReleased()),
            map, SLOT(gameResume()));

    // let generation time slider change time interval
    connect(ui->generationTimeSlider, SIGNAL(valueChanged(int)),
            map, SLOT(setInterval(int)));

    connect(ui->colonySizeSpinBox, SIGNAL(editingFinished()),
            this, SLOT(setMapSize()));
    connect(ui->generationTimeSpinBox, SIGNAL(editingFinished()),
            this, SLOT(setInterval()));

    // Buttons must react to game running state
    connect(map, SIGNAL(gameRunning(bool)),
            ui->stepButton, SLOT(setDisabled(bool)));
    connect(map, SIGNAL(gameRunning(bool)),
            ui->startButton, SLOT(setDisabled(bool)));
    connect(map, SIGNAL(gameRunning(bool)),
            ui->stopButton, SLOT(setEnabled(bool)));

    // inform sliders, when size and tine interval changed
    connect(map, SIGNAL(sizeChanged(int)),
            ui->colonySizeSlider, SLOT(setValue(int)));
    connect(map, SIGNAL(intervalChanged(int)),
            ui->generationTimeSlider, SLOT(setValue(int)));

    connect(map, SIGNAL(generationLived(int)),
            ui->generationCountLabel, SLOT(setNum(int)));
}

GameOfLifeMain::~GameOfLifeMain()
{
    delete ui;
}

void GameOfLifeMain::setMapSize()
{
    map->setMapSize(ui->colonySizeSpinBox->value());
}

void GameOfLifeMain::setInterval()
{
    map->setInterval(ui->generationTimeSpinBox->value());
}
