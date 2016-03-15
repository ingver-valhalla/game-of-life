#include "colonymap.h"
#include <QMessageBox>

ColonyMap::ColonyMap(QWidget *parent, int size, int interval)
    : QWidget(parent),
      mapSize(size),
      cellColor("#002a77"),
      timer(new QTimer(this)),
      curGenMap(new Map(mapSize, MapRow(mapSize))),
      nextGenMap(new Map(mapSize, MapRow(mapSize))),
      visitedCells(new Map(mapSize, MapRow(mapSize))),
      originalMap(new Map(mapSize, MapRow(mapSize))),
      originalVisited(new Map(mapSize, MapRow(mapSize))),
      running(false)
{
    timer->setInterval(interval);
    emit sizeChanged(mapSize);
    emit intervalChanged(timer->interval());
    connect(timer, SIGNAL(timeout()), this, SLOT(nextGen()));

    // setting size policy
//    QSizePolicy newSizePolicy = sizePolicy();
//    newSizePolicy.setHeightForWidth(true);
//    setSizePolicy(newSizePolicy);
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

// EVENTS

void ColonyMap::mousePressEvent(QMouseEvent *e)
{
//    qDebug() << "clicked on grid";

    if(e->buttons() & Qt::LeftButton) {
        qreal cellHeight = (qreal)height() / mapSize;
        qreal cellWidth = (qreal)width() / mapSize;

        int row = qFloor(e->y() / cellHeight);
        int col = qFloor(e->x() / cellWidth);

        setCell(curGenMap, row, col, !getCell(curGenMap, row, col));
        update();
    }
    // for debugging
    // must be deleted
    if(e->buttons() & Qt::RightButton) {
        if(timer->isActive()) {
            timer->stop();
        }
        else {
            timer->start();
        }
//        nextGen();
    }
}

void ColonyMap::mouseMoveEvent(QMouseEvent *e)
{
    if(e->buttons() & Qt::LeftButton) {
        qreal cellHeight = (qreal)height() / mapSize;
        qreal cellWidth = (qreal)width() / mapSize;

        int row = qFloor(e->y() / cellHeight);
        int col = qFloor(e->x() / cellWidth);

        setCell(curGenMap, row, col, true);
        update();
    }
}

void ColonyMap::paintEvent(QPaintEvent *)
{
//    static int d = 1;
//    qDebug() << "repainting";
    QPainter canvas(this);
//    canvas.setWorldTransform(QTransform().translate(d,d));
//    qDebug() << canvas.worldTransform();
//    ++d;
    drawGrid(canvas);
    drawColony(canvas);
}

void ColonyMap::timerEvent(QTimerEvent *)
{
//    qDebug() << "step on timeout";
    nextGen();
}

void ColonyMap::drawGrid(QPainter &canvas)
{
    qreal cellHeight = (qreal)height() / mapSize;
    qreal cellWidth = (qreal)width() / mapSize;

    QColor gridColor(cellColor);
    gridColor.setAlpha(10);
    canvas.setPen(gridColor);
//    qDebug() << "gridColor.isValid():" << gridColor.isValid();
//    qDebug() << "cellColor:" << cellColor;
//    qDebug() << "gridColor:" << gridColor;

//    qDebug() << "ColonyMap::drawGrid:drawing grid";
    for(qreal x = cellWidth; x <= width(); x += cellWidth) {
        QLineF vertLine(x, 0, x, height());
        canvas.drawLine(vertLine);
    }
    for(qreal y = cellHeight; y <= height(); y += cellHeight) {
        QLineF horizLine(0, y, width(), y);
        canvas.drawLine(horizLine);
    }

//    qDebug() << "ColonyMap::drawGrid: drawing borders";
    // top
    canvas.drawLine(0, 0, width() - 1, 0);
    // left
    canvas.drawLine(0, 0, 0, height()-1);
    // right
    canvas.drawLine(width()-1, 0, width()-1, height()-1);
    // bottom
    canvas.drawLine(0, height()-1, width()-1, height()-1);
}

void ColonyMap::drawColony(QPainter &canvas)
{
//    qDebug() << "drawing colony";

    canvas.setBrush(cellColor);
    QColor visitedColor(cellColor);
    visitedColor.setAlpha(50);

    qreal cellHeight = (qreal)height() / mapSize;
    qreal cellWidth = (qreal)width() / mapSize;

    for(int row = 0; row < mapSize; ++row) {
        for(int col = 0; col < mapSize; ++col) {
            if(getCell(visitedCells, row, col)) {
                QRectF boundary(col*cellWidth, row*cellHeight, cellWidth, cellHeight);
                canvas.fillRect(boundary, visitedColor);
            }
            if(getCell(curGenMap, row, col)) {
                QRectF boundary(col*cellWidth, row*cellHeight, cellWidth, cellHeight);
                canvas.drawEllipse(boundary);
            }

        }
    }
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

    return (neighbors == 2 && getCell(curGenMap, row, col)) || (neighbors == 3);
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
    emit sizeChanged(size);
    mapSize = size;

    QVector<QBitArray> *t1 = curGenMap;
    QVector<QBitArray> *t2 = nextGenMap;
    QVector<QBitArray> *t3 = visitedCells;

    curGenMap = new QVector<QBitArray>(mapSize, QBitArray(mapSize));
    nextGenMap = new QVector<QBitArray>(mapSize, QBitArray(mapSize));
    visitedCells = new QVector<QBitArray>(mapSize, QBitArray(mapSize));

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
            setCell(curGenMap, row + offset1, col + offset1, getCell(t1, row + offset2, col + offset2));
            setCell(nextGenMap, row + offset1, col + offset1, getCell(t2, row + offset2, col + offset2));
            setCell(visitedCells, row + offset1, col + offset1, getCell(t3, row + offset2, col + offset2));
        }
    }

    delete t1;
    delete t2;
    delete t3;
    update();
}

void ColonyMap::nextGen()
{
    // if nothing changed since last turn
    if(*curGenMap == *nextGenMap) {
        gameStop();
        QMessageBox::information(this, "Game Over", "Game Over");
        return;
    }

    for(int row = 0; row < mapSize; ++row) {
        for(int col = 0; col < mapSize; ++col) {
            if(getCell(curGenMap, row, col))
                setCell(visitedCells, row, col, true);
        }
    }
    for(int row = 0; row < mapSize; ++row) {
        for(int col = 0; col < mapSize; ++col) {
            setCell(nextGenMap, row, col, stillLive(row, col));
        }
    }
    QVector<QBitArray> *temp = curGenMap;
    curGenMap = nextGenMap;
    nextGenMap = temp;
    update();
}

void ColonyMap::setInterval(int interval)
{
    timer->setInterval(interval);
    intervalChanged(interval);
}

void ColonyMap::gameStart()
{    
    running = true;
    if(!timer->isActive()) {
        emit started();
        emit gameRunning(true);
        timer->start();
    }
}

void ColonyMap::gameStop()
{
    emit stopped();
    emit gameRunning(false);
    running = false;
    timer->stop();
}

void ColonyMap::gamePause()
{
    if(running) {
        emit paused();
        emit gameRunning(false);
        timer->stop();
    }
}

void ColonyMap::gameResume()
{
    if(running) {
        emit unpaused();
        emit gameRunning(true);
        if(!timer->isActive())
            timer->start();
    }
}

void ColonyMap::gameReset()
{
    emit stopped();
    running = false;
    timer->stop();
    setMapSize(originalMap->size());
    *curGenMap = *originalMap;
    *visitedCells = *originalVisited;
    update();
}

void ColonyMap::setCellColor(QColor &newColor)
{
    emit colorChanged(newColor);
    cellColor = newColor;
    update();
}

void ColonyMap::saveMap(QDataStream &out)
{
    gamePause();
    qDebug() << "ColonyMap::saveMap()";

    if(!mapEmpty(curGenMap)) {
        out << *curGenMap << *visitedCells << cellColor;
    }

    *originalMap = *curGenMap;
    *originalVisited = *visitedCells;

    gameResume();
}

void ColonyMap::loadMap(QDataStream &in)
{
    gameStop();

    QVector<QBitArray> tempMap1;
    QVector<QBitArray> tempMap2;
    QColor tempColor;

    in >> tempMap1;
    in >> tempMap2;
    in >> tempColor;

    if(tempMap1.size() != tempMap2.size()
       || !tempColor.isValid())
    {
        QMessageBox::information(this, "Error", "Map is broken");
        return;
    }

    mapSize = tempMap1.size();
    cellColor = tempColor;
    emit colorChanged(cellColor);
    *curGenMap = tempMap1;
    delete nextGenMap;
    nextGenMap = new Map(mapSize, MapRow(mapSize));
    *visitedCells = tempMap2;
    *originalMap = *curGenMap;
    *originalVisited = *visitedCells;

    update();
}
