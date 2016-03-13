#include "colonymap.h"
#include <QMessageBox>

ColonyMap::ColonyMap(QWidget *parent, uint height, uint width)
    : QWidget(parent),
      mapHeight(height),
      mapWidth(width),
      timer(new QTimer(this)),
      curGenMap(new QVector<QBitArray>(mapHeight, QBitArray(mapWidth))),
      nextGenMap(new QVector<QBitArray>(mapHeight, QBitArray(mapWidth)))
{
    timer->setInterval(100);
    connect(timer, SIGNAL(timeout()), this, SLOT(nextGen()));
}

ColonyMap::~ColonyMap()
{
    delete curGenMap;
    delete nextGenMap;
}

// EVENTS

void ColonyMap::mousePressEvent(QMouseEvent *e)
{
    qDebug() << "clicked on grid";

    if(e->buttons() & Qt::LeftButton) {
        qreal cellHeight = (qreal)height() / mapHeight;
        qreal cellWidth = (qreal)width() / mapWidth;

        uint row = qFloor(e->y() / cellHeight);
        uint col = qFloor(e->x() / cellWidth);

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
        qreal cellHeight = (qreal)height() / mapHeight;
        qreal cellWidth = (qreal)width() / mapWidth;

        uint row = qFloor(e->y() / cellHeight);
        uint col = qFloor(e->x() / cellWidth);

        setCell(curGenMap, row, col, true);
        update();
    }

}

void ColonyMap::paintEvent(QPaintEvent *)
{
    qDebug() << "repainting";
    QPainter canvas(this);
    drawGrid(canvas);
    drawColony(canvas);
}

void ColonyMap::timerEvent(QTimerEvent *)
{
    qDebug() << "step on timeout";
    nextGen();
}

void ColonyMap::drawGrid(QPainter &canvas)
{
    qreal cellHeight = (qreal)height() / mapHeight;
    qreal cellWidth = (qreal)width() / mapWidth;

    canvas.setPen(cellColor);

    qDebug() << "ColonyMap::drawGrid:drawing grid";
    for(qreal x = cellWidth; x <= width(); x += cellWidth) {
        canvas.drawLine(x, 0, x, height());
    }
    for(qreal y = cellHeight; y <= height(); y += cellHeight) {
        canvas.drawLine(0, y, width(), y);
    }

    qDebug() << "ColonyMap::drawGrid: drawing borders";
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
    qDebug() << "drawing colony";

    canvas.setBrush(Qt::SolidPattern);

    qreal cellHeight = (qreal)height() / mapHeight;
    qreal cellWidth = (qreal)width() / mapWidth;

    for(uint row = 0; row < mapHeight; ++row) {
        for(uint col = 0; col < mapWidth; ++col) {
            if(getCell(curGenMap, row, col)) {
                canvas.drawEllipse(col * cellWidth, row * cellHeight, cellWidth, cellHeight);
            }
        }
    }
}

void ColonyMap::setCell(QVector<QBitArray> * map, uint row, uint col, bool val)
{
    if(map == NULL || row >= mapHeight || col >= mapWidth) {
        return;
    }

    (*map)[row][col] = val;
}

bool ColonyMap::getCell(QVector<QBitArray> * map, uint row, uint col)
{
    if(map == NULL || row >= mapHeight || col >= mapWidth) {
        return false;
    }

    return (*map)[row][col];
}

bool ColonyMap::stillLive(uint row, uint col)
{
    if(row >= mapHeight || col >= mapWidth) {
        throw "stillLive(): invalid indeces";
    }

    int neighbors = 0;

    // counting neighbors
    // upper-left
    neighbors += getCell(curGenMap,
                         row ? row-1 : mapHeight-1,
                         col ? col-1 : mapWidth-1);
    // upper
    neighbors += getCell(curGenMap,
                         row ? row-1 : mapHeight-1,
                         col);
    // upper-right
    neighbors += getCell(curGenMap,
                         row ? row-1 : mapHeight-1,
                         col < mapWidth-1 ? col+1 : 0);
    // right
    neighbors += getCell(curGenMap,
                         row,
                         col < mapWidth-1 ? col+1 : 0);
    // lower-right
    neighbors += getCell(curGenMap,
                         row < mapHeight-1 ? row+1 : 0,
                         col < mapWidth-1 ? col+1 : 0);
    // lower
    neighbors += getCell(curGenMap,
                         row < mapHeight-1 ? row+1 : 0,
                         col);
    // lower-left
    neighbors += getCell(curGenMap,
                         row < mapHeight-1 ? row+1 : 0,
                         col ? col-1 : mapWidth-1);
    // left
    neighbors += getCell(curGenMap,
                         row,
                         col ? col-1 : mapWidth-1);

    return (neighbors == 2 && getCell(curGenMap, row, col)) || (neighbors == 3);
}

// SLOTS
void ColonyMap::setMapHeight(uint height)
{
    mapHeight = height;
    delete curGenMap;
    delete nextGenMap;
    curGenMap = new QVector<QBitArray>(mapHeight, QBitArray(mapWidth));
    nextGenMap = new QVector<QBitArray>(mapHeight, QBitArray(mapWidth));
}

void ColonyMap::setMapWidth(uint width)
{
    mapWidth = width;
    delete curGenMap;
    delete nextGenMap;
    curGenMap = new QVector<QBitArray>(mapHeight, QBitArray(mapWidth));
    nextGenMap = new QVector<QBitArray>(mapHeight, QBitArray(mapWidth));
}

void ColonyMap::nextGen()
{
    // if nothing changed since last turn
    if(*curGenMap == *nextGenMap) {
        timer->stop();
        QMessageBox::information(this, "Game Over", "Game Over");
        return;
    }

    for(uint row = 0; row < mapHeight; ++row) {
        for(uint col = 0; col < mapWidth; ++col) {
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
}
