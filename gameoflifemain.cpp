#include "gameoflifemain.h"
#include "ui_gameoflifemain.h"
#include <QColorDialog>
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>
#include <QSizePolicy>

GameOfLifeMain::GameOfLifeMain(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GameOfLifeMain),
    map(new ColonyMap(this))

{
    ui->setupUi(this);

    // place colony widget on window
    ui->mapLayout->addWidget(map);
    ui->mainLayout->setStretchFactor(ui->mapLayout, 2);
    ui->mainLayout->setStretchFactor(ui->settingsLayout, 1);

    // setting connections

    // connect control buttons to main slots of map
    connect(ui->playButton, SIGNAL(clicked(bool)), map, SLOT(gameStart()));
    connect(ui->stopButton, SIGNAL(clicked(bool)), map, SLOT(gameStop()));
    connect(ui->stepButton, SIGNAL(clicked(bool)), map, SLOT(nextGen()));
    connect(ui->resetButton, SIGNAL(clicked(bool)), map, SLOT(gameReset()));
    connect(ui->cleanColonyButton, SIGNAL(clicked(bool)), map, SLOT(cleanMap()));

    // let size slider set map size
    connect(ui->colonySizeSlider, SIGNAL(valueChanged(int)), map, SLOT(setMapSize(int)));
    // while slider pressed, pause the game
    connect(ui->colonySizeSlider, SIGNAL(sliderPressed()), map, SLOT(gamePause()));
    // resume, when slider released
    connect(ui->colonySizeSlider, SIGNAL(sliderReleased()), map, SLOT(gameResume()));

    // let generation time slider change time interval
    connect(ui->generationTimeSlider, SIGNAL(valueChanged(int)), map, SLOT(setInterval(int)));
    // inform sliders, when size and tine interval changed
    connect(map, SIGNAL(sizeChanged(int)), ui->colonySizeSlider, SLOT(setValue(int)));
    connect(map, SIGNAL(intervalChanged(int)),ui->generationTimeSlider, SLOT(setValue(int)));

    // Step button disabled when game is running
    connect(map, SIGNAL(gameRunning(bool)), ui->stepButton, SLOT(setDisabled(bool)));

    // saving and loading
    connect(ui->saveButton, SIGNAL(clicked(bool)), this, SLOT(saveMap()));
    connect(ui->loadButton, SIGNAL(clicked(bool)), this, SLOT(loadMap()));
    // cell color control
    connect(ui->cellColorButton, SIGNAL(clicked(bool)), this, SLOT(setMapColor()));

    connect(ui->brushInverseCheckBox, SIGNAL(toggled(bool)), map, SLOT(setBrushInverse(bool)));

    connect(ui->colonySizeSpinBox, SIGNAL(editingFinished()), this, SLOT(setMapSize()));
    connect(ui->generationTimeSpinBox, SIGNAL(editingFinished()), this, SLOT(setInterval()));
}

GameOfLifeMain::~GameOfLifeMain()
{
    delete ui;
}

void GameOfLifeMain::saveMap()
{
    map->gamePause();
    if(map->isEmpty()) {
        qDebug() << "nothing to save";
        QMessageBox::information(this, "Error", "Colony is empty");
        map->gameResume();
        return;
    }

    qDebug() << "saving map";
    QString fileName = QFileDialog::getSaveFileName(this, "Open map file", QDir::homePath(), "Life Map (*.lifemap);; All (*)");
    if(fileName.isNull()) {
        qDebug() << "Filename is Null";
        map->gameResume();
        return;
    }
    qDebug() << "Filename is" << fileName;
    QFile saveFile(fileName);
    if(!saveFile.open(QIODevice::WriteOnly)) {
        qDebug() << "GameOfLifeMain::saveMap(): Can't open file";
        QMessageBox::information(this, "Error", "Can't open file");
        map->gameResume();
        return;
    }
    QDataStream out(&saveFile);
    map->saveMap(out);
}

void GameOfLifeMain::loadMap()
{
    map->gamePause();

    qDebug() << "loading map";
    QString fileName = QFileDialog::getOpenFileName(this, "Open map file", QDir::homePath(), "Life Map (*.lifemap);; All (*)");
    if(fileName.isNull()) {
        qDebug() << "Filename is Null";
        map->gameResume();
        return;
    }
    qDebug() << "Filename is" << fileName;
    QFile loadFile(fileName);
    if(!loadFile.open(QIODevice::ReadOnly)) {
        qDebug() << "GameOfLifeMain::loadMap(): Can't open file";
        QMessageBox::information(this, "Error", "Can't open file");
        map->gameResume();
        return;
    }
    QDataStream in(&loadFile);

    map->loadMap(in);
}

void GameOfLifeMain::setMapColor()
{
    map->gamePause();
    QColor newColor = QColorDialog::getColor();
    if(!newColor.isValid())
        return;
    map->setCellColor(newColor);
    map->gameResume();
}

void GameOfLifeMain::setMapSize()
{
    map->setMapSize(ui->colonySizeSpinBox->value());
}

void GameOfLifeMain::setInterval()
{
    map->setInterval(ui->generationTimeSpinBox->value());
}
