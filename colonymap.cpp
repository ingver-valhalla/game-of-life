#include "colonymap.h"
#include <QMessageBox>
#include <QLayout>
#include <QColorDialog>
#include <QFileDialog>
#include <QMessageBox>

ColonyMap::ColonyMap(QWidget *parent, int size, int interval)
    : QWidget(parent),
      timer(new QTimer(this)),

      mapSize(size),
      cellColor("#002a77"),
      curGenMap(new Map(mapSize, MapRow(mapSize))),
      nextGenMap(new Map(mapSize, MapRow(mapSize))),
      visitedCells(new Map(mapSize, MapRow(mapSize))),
      generationCount(0),

      originalMap(new Map(mapSize, MapRow(mapSize))),
      originalVisited(new Map(mapSize, MapRow(mapSize))),
      originalCount(0),

      running(false)
{
    timer->setInterval(interval);
    emit intervalChanged(timer->interval());
    emit sizeChanged(mapSize);
    emit colorChanged(cellColor);
    emit gameRunning(running);

    connect(timer, SIGNAL(timeout()), this, SLOT(nextGen()));
    setMouseTracking(true);
}

ColonyMap::~ColonyMap()
{
    delete curGenMap;
    delete nextGenMap;
    delete visitedCells;
    delete originalMap;
}

bool ColonyMap::isRunning()
{
    return running;
}

bool ColonyMap::isEmpty()
{
    return mapEmpty(curGenMap);
}

void ColonyMap::setGenerationCount(int newCount)
{
    generationCount = newCount;
    emit generationLived(generationCount);
}

// EVENTS

void ColonyMap::mousePressEvent(QMouseEvent *e)
{
    if(e->buttons() | Qt::LeftButton | Qt::RightButton) {
        qreal cellHeight = (qreal)height() / mapSize;
        qreal cellWidth = (qreal)width() / mapSize;

        int row = qFloor(e->y() / cellHeight);
        int col = qFloor(e->x() / cellWidth);
        lastCell = QPoint(col, row);

        if(e->buttons() & Qt::LeftButton) {
            setCell(curGenMap, row, col, true);
        } else if(e->buttons() & Qt::RightButton) {
            setCell(curGenMap, row, col, false);
        }
        update();
    }
}

void ColonyMap::mouseMoveEvent(QMouseEvent *e)
{
    qreal cellHeight = (qreal)height() / mapSize;
    qreal cellWidth = (qreal)width() / mapSize;

    int row = qFloor(e->y() / cellHeight);
    int col = qFloor(e->x() / cellWidth);

    if(e->buttons() & Qt::LeftButton) {
        if(row != lastCell.y() || col != lastCell.x()) {
            lastCell = QPoint(col, row);
            setCell(curGenMap, row, col, true);
        }
    }
    else if(e->buttons() & Qt::RightButton) {
        setCell(curGenMap, row, col, false);
    }
    else if(!running) {
        shadowCell = QPoint(col, row);
    }
    update();
}

void ColonyMap::paintEvent(QPaintEvent *)
{
    QPainter canvas(this);

    drawGrid(canvas);
    drawColony(canvas);
}

void ColonyMap::drawGrid(QPainter &canvas)
{
    qreal cellHeight = (qreal)height() / mapSize;
    qreal cellWidth = (qreal)width() / mapSize;

    QColor gridColor(cellColor);
    canvas.setPen(gridColor);

    canvas.drawLine(0, 0, width() - 1, 0); // top
    canvas.drawLine(0, 0, 0, height()-1); // left
    canvas.drawLine(width()-1, 0, width()-1, height()-1); // right
    canvas.drawLine(0, height()-1, width()-1, height()-1); // bottom

    gridColor.setAlpha(10);
    canvas.setPen(gridColor);

    for(qreal x = cellWidth; x <= width(); x += cellWidth) {
        QLineF vertLine(x, 0, x, height());
        canvas.drawLine(vertLine);
    }
    for(qreal y = cellHeight; y <= height(); y += cellHeight) {
        QLineF horizLine(0, y, width(), y);
        canvas.drawLine(horizLine);
    }


}

void ColonyMap::drawColony(QPainter &canvas)
{
    canvas.setBrush(cellColor);
    QColor visitedColor(cellColor);
    visitedColor.setAlpha(50);

    qreal cellHeight = (qreal)height() / mapSize;
    qreal cellWidth = (qreal)width() / mapSize;

    for(int row = 0; row < mapSize; ++row) {
        for(int col = 0; col < mapSize; ++col) {
            if(getCell(visitedCells, row, col)) {
                QRectF boundary(col*cellWidth, row*cellHeight,
                                cellWidth, cellHeight);
                canvas.fillRect(boundary, visitedColor);
            }
            if(getCell(curGenMap, row, col)) {
                QRectF boundary(
                    col*cellWidth,
                    row*cellHeight,
                    cellWidth, cellHeight);
                canvas.drawEllipse(boundary);
            }

        }
    }
    if(!running && !shadowCell.isNull()) {
        QColor c = cellColor;
        c.setAlpha(100);
        canvas.setBrush(c);
        QRectF boundary(
            shadowCell.x()*cellWidth,
            shadowCell.y()*cellHeight,
            cellWidth, cellHeight);
        canvas.drawEllipse(boundary);
        shadowCell = QPoint();
    }

}

void ColonyMap::resizeEvent(QResizeEvent *)
{
    int newWidth = geometry().width();
    int newHeight = geometry().height();
    int newX = geometry().x();
    int newY = geometry().y();

    int leftMargin;
    int topMargin;
    parentWidget()->layout()->getContentsMargins(
        &leftMargin, &topMargin, NULL, NULL);

    if(newWidth > newHeight) {
        newWidth = newHeight;
        newX = (geometry().width() - newWidth) / 2 + leftMargin;
    }
    else {
        newHeight = newWidth;
        newY = (geometry().height() - newHeight) + topMargin;
    }

    setGeometry(newX, newY, newWidth, newHeight);
}

void ColonyMap::setCell(Map *map, int row, int col, bool val)
{
    if(map == NULL
       || row < 0 || row >= mapSize
       || col < 0 || col >= mapSize)
    {
        return;
    }

    (*map)[row][col] = val;
}

bool ColonyMap::getCell(Map *map, int row, int col)
{
    if(map == NULL
       || row < 0 || row >= mapSize
       || col < 0 || col >= mapSize)
    {
        return false;
    }

    return (*map)[row][col];
}

bool ColonyMap::stillLive(int row, int col)
{
    if(row < 0 || row >= mapSize
       || col < 0 || col >= mapSize)
    {
        throw "stillLive(): invalid indeces";
    }

    int neighbors = 0;

    // counting neighbors
    // upper-left
    neighbors += getCell(curGenMap,
                         row ? row-1 : mapSize-1,
                         col ? col-1 : mapSize-1);
    // upper
    neighbors += getCell(curGenMap,
                         row ? row-1 : mapSize-1,
                         col);
    // upper-right
    neighbors += getCell(curGenMap,
                         row ? row-1 : mapSize-1,
                         col < mapSize-1 ? col+1 : 0);
    // right
    neighbors += getCell(curGenMap,
                         row,
                         col < mapSize-1 ? col+1 : 0);
    // lower-right
    neighbors += getCell(curGenMap,
                         row < mapSize-1 ? row+1 : 0,
                         col < mapSize-1 ? col+1 : 0);
    // lower
    neighbors += getCell(curGenMap,
                         row < mapSize-1 ? row+1 : 0,
                         col);
    // lower-left
    neighbors += getCell(curGenMap,
                         row < mapSize-1 ? row+1 : 0,
                         col ? col-1 : mapSize-1);
    // left
    neighbors += getCell(curGenMap,
                         row,
                         col ? col-1 : mapSize-1);

    return (neighbors == 2 && getCell(curGenMap, row, col)) ||
        (neighbors == 3);
}

bool ColonyMap::mapEmpty(Map *map)
{
    for(int row = 0; row < mapSize; ++row) {
        for(int col = 0; col < mapSize; ++col) {
            if(getCell(map, row, col))
                return false;
        }
    }
    return true;
}

// SLOTS
void ColonyMap::setMapSize(int size)
{
    mapSize = size;

    Map *t1 = curGenMap;
    Map *t2 = nextGenMap;
    Map *t3 = visitedCells;

    curGenMap = new Map(mapSize, MapRow(mapSize));
    nextGenMap = new Map(mapSize, MapRow(mapSize));
    visitedCells = new Map(mapSize, MapRow(mapSize));

    int minSize;
    int offset1;
    int offset2;
    if(t1->size() < mapSize) {
        minSize = t1->size();
        offset1 = (mapSize - minSize) / 2;
        offset2 = 0;
    }
    else {
        minSize = mapSize;
        offset1 = 0;
        offset2 = (t1->size() - minSize) / 2;
    }
    for(int row = 0; row < minSize; ++row) {
        for(int col = 0; col < minSize; ++col) {
            setCell(curGenMap, row + offset1, col + offset1,
                getCell(t1, row + offset2, col + offset2));
            setCell(nextGenMap, row + offset1, col + offset1,
                getCell(t2, row + offset2, col + offset2));
            setCell(visitedCells, row + offset1, col + offset1,
                getCell(t3, row + offset2, col + offset2));
        }
    }

    delete t1;
    delete t2;
    delete t3;
    update();

    emit sizeChanged(size);
}

void ColonyMap::nextGen()
{
    for(int row = 0; row < mapSize; ++row) {
        for(int col = 0; col < mapSize; ++col) {
            setCell(nextGenMap, row, col, stillLive(row, col));
            if (getCell(curGenMap, row, col)) {
                setCell(visitedCells, row, col, true);
            }
        }
    }

    // if nothing changed since last turn
    if(*curGenMap == *nextGenMap) {
        gameStop();
        QMessageBox::information(this, "Game Over", "Game Over");
        return;
    }
    Map *temp = curGenMap;
    curGenMap = nextGenMap;
    nextGenMap = temp;
    update();

    setGenerationCount(generationCount + 1);
}

void ColonyMap::setInterval(int interval)
{
    timer->setInterval(interval);
    emit intervalChanged(interval);
}

void ColonyMap::gameStart()
{    
    running = true;
    if(!timer->isActive()) {
        emit gameRunning(true);
        timer->start();
        setMouseTracking(false);
    }
}

void ColonyMap::gameStop()
{
    emit gameRunning(false);
    running = false;
    timer->stop();
    setMouseTracking(true);
}

void ColonyMap::gamePause()
{
    if(running) {
        emit gamePaused();
        timer->stop();
    }
}

void ColonyMap::gameResume()
{
    if(running && !timer->isActive()) {
        emit gameResumed();
        timer->start();
    }
}

void ColonyMap::gameReset()
{
    emit gameRunning(false);
    running = false;
    timer->stop();

    // restore original values
    setMapSize(originalMap->size());    
    *curGenMap = *originalMap;
    *visitedCells = *originalVisited;

    update();

    setGenerationCount(originalCount);
}

void ColonyMap::chooseCellColor()
{
    gamePause();
    QColor newColor = QColorDialog::getColor();
    setCellColor(newColor);
    gameResume();
}

void ColonyMap::setCellColor(const QColor& newColor)
{
    if(!newColor.isValid())
        return;
    cellColor = newColor;
    update();

    emit colorChanged(newColor);
}

void ColonyMap::saveMap()
{
   gamePause();
    if(isEmpty()) {
        QMessageBox::information(this, "Error", "Colony is empty");
        gameResume();
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(
                this,
                "Save map file",
                QDir::homePath(),
                "Life Map (*.lifemap);; All (*)");
    if(fileName.isNull()) {
        gameResume();
        return;
    }
    QFile saveFile(fileName);
    if(!saveFile.open(QIODevice::WriteOnly)) {
        QMessageBox::information(this, "Error", "Can't open file");
        gameResume();
        return;
    }
    QDataStream out(&saveFile);
    out << *curGenMap << *visitedCells << cellColor << generationCount;

    *originalMap = *curGenMap;
    *originalVisited = *visitedCells;
    originalCount = generationCount;

    emit mapSaved();

    gameResume();
}

void ColonyMap::loadMap()
{
    gamePause();

    QString fileName = QFileDialog::getOpenFileName(
                this,
                "Open map file",
                QDir::homePath(),
                "Life Map (*.lifemap);; All (*)");
    if(fileName.isNull()) {
        gameResume();
        return;
    }
    QFile loadFile(fileName);
    if(!loadFile.open(QIODevice::ReadOnly)) {
        QMessageBox::information(this, "Error", "Can't open file");
        gameResume();
        return;
    }

    gameStop();

    QDataStream in(&loadFile);

    Map tempMap1;
    Map tempMap2;
    QColor tempColor;
    int tempCount;

    in >> tempMap1;
    in >> tempMap2;
    in >> tempColor;
    in >> tempCount;

    if(tempMap1.size() != tempMap2.size()
       || !tempColor.isValid()
       || in.status() == QDataStream::ReadCorruptData
       || in.status() == QDataStream::ReadPastEnd)
    {
        QMessageBox::information(this, "Error", "Map is broken");
        return;
    }

    setMapSize(tempMap1.size());
    setCellColor(tempColor);
    *curGenMap = tempMap1;
    delete nextGenMap;
    nextGenMap = new Map(mapSize, MapRow(mapSize));
    *visitedCells = tempMap2;

    // save original values
    *originalMap = *curGenMap;
    *originalVisited = *visitedCells;
    originalCount = tempCount;

    update();
    emit mapLoaded();

    setGenerationCount(tempCount);
}

void ColonyMap::cleanMap()
{
    gameStop();
    delete curGenMap;
    delete nextGenMap;
    delete visitedCells;
    delete originalMap;
    delete originalVisited;

    curGenMap = new Map(mapSize, MapRow(mapSize));
    nextGenMap = new Map(mapSize, MapRow(mapSize));
    visitedCells = new Map(mapSize, MapRow(mapSize));
    originalMap = new Map(mapSize, MapRow(mapSize));
    originalVisited = new Map(mapSize, MapRow(mapSize));
    setGenerationCount(0);
    update();

    emit mapCleaned();
}
